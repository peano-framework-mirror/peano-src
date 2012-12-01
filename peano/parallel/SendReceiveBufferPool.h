// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_PARALLEL_SEND_RECEIVE_BUFFER_POOL_H_
#define _PEANO_PARALLEL_SEND_RECEIVE_BUFFER_POOL_H_


#include "tarch/logging/Log.h"
#include "tarch/services/Service.h"
#include "tarch/compiler/CompilerSpecificSettings.h"
#include "peano/parallel/SendReceiveBuffer.h"


#include <map>

namespace peano {
  namespace parallel {
    class SendReceiveBufferPool;
  }
}


/**
 * Send/Receive Buffer Pool
 *
 * This class is the single point of contact to exchange PDE-specific data.
 * It stores all the SendReceiveBuffer instances belonging to one node.
 *
 * Furthermore, the class is responsible for distributing all the vertices
 * among the different receive buffers, i.e. it analyses the vertex's
 * information which subdomains are adjacent. Consequently, you may not pass it
 * vertices that are not part of the parallel boundary.
 *
 * The buffer management is a lazy management, i.e. buffers required are
 * created on demand.
 *
 * @author Tobias Weinzierl
 */
class peano::parallel::SendReceiveBufferPool: public tarch::services::Service {
  public:
    enum BufferAccessType {
      LIFO,FIFO
    };
  private:
    static tarch::logging::Log _log;

    /**
     * Set by the constructor and then never changed again.
     */
    int  _iterationManagementTag;

    /**
     * Set by the constructor and then never changed again.
     */
    int  _iterationDataTag;

    /**
     * Maps ranks to buffers.
     */
    std::map<int,SendReceiveBuffer*> _map;

    /**
     * By default 1.
     */
    int _bufferSize;

    SendReceiveBufferPool();


  public:
    ~SendReceiveBufferPool();

    static SendReceiveBufferPool& getInstance();

    /**
     * Means that all buffers are cleared and freed (deleted).
     */
    void terminate();

    /**
     * Create buffer manually
     *
     * If we have a spacetree node with a new master, the following situation
     * might occur (and does occur):
     *
     * - The master books the new worker and finishes this iteration
     * - The master starts the new iteration and writes several messages (both
     *   boundary and fork) to the new worker
     * - The master sends the startup message
     * - The master continues to send boundary and fork/join data to its worker
     *
     * If the worker now cannot handle that many messages as it runs out of
     * buffers, this startup messages is somehow hidden in all the fork and
     * boundary data. At the same time, the worker has not ever received any
     * data from the master, i.e. it hasn't even created the corresponding
     * buffers. We'll thus run into a deadlock.
     *
     * @param toRank Usually always the master process if you call it
     *               externally. However, I also use this method inside the
     *               class and there it may have a different value.
     */
    template <class Vertex>
    void createBufferManually(int toRank, const BufferAccessType& bufferAccessType );
    
    /**
     * Restart the Node Pool.
     *
     * As the buffer implements a lazy behaviour, this operation does not create
     * new buffers. However, it sets back the buffer size to one. For each
     * restart(), the user has to call terminate() before. Consequently,
     * the maps with the send and receive buffers has to be empty when restart()
     * is invoked. For the Peano repositories this implies the following
     * constraint: the initialisation of the (regular grid) data containers
     * sends away vertices. Hence, the initialisation implies the creation of
     * send buffers. Hence, the reset of the pool has to be done #before# the
     * data container is initialised.
     */
    void restart();

    /**
     * This tag is used to send and receive the states and cells throughout
     * normal iterations. The repository states, i.e. which adapter to use,
     * are also sent due to this tag. Cells are not exchanged by the regular
     * grid.
     */
    int getIterationManagementTag() const;

    /**
     * Exchange data of the non-overlapping boundary.
     */
    int getIterationDataTag() const;

    /**
     * Poll the MPI queues whether there are messages pending. If so, take them
     * and insert them into the local queue.
     *
     * Right now, the operation only searches for messages from buffers where
     * it knows, that the local node exchanges messages with this rank. However,
     * it might be that there are already messages from other nodes (due to an
     * additional fork or join) that are not yet known to be communication
     * partners. The original code of Peano 1 took care of this behaviour and
     * inserted something like:
     * \code

  if (!receivedPage) {
    MPI_Status status;
    int        flag;
    int probeResult = MPI_Iprobe(
      MPI_ANY_SOURCE,
      Vertex::DataExchangeTag,
      Node::getInstance().getCommunicator(),
      &flag, &status
    );
    if (probeResult!=MPI_SUCCESS) {
      std::ostringstream msg;
      msg << "probing for dangling messages failed: "
          << MPIReturnValueToString(probeResult);
      _log.error("receiveDanglingMessages()", msg.str() );
    }
    if (flag && _map.count(status.MPI_SOURCE)==0 ) {
      #ifdef Debug
      std::ostringstream msg;
      msg << "there's a message from node " << status.MPI_SOURCE
          << ", but there's no buffer. Create buffer";
      _log.debug("receiveDanglingMessages()", msg.str() );
      #endif
      createBuffer( status.MPI_SOURCE );
      _map[ status.MPI_SOURCE ].receivePageIfAvailable();
    }
  }

  \endcode
     *
     * It did not transfer this piece of code into Peano's second release, as I
     * have to know the vertex type to create a new buffer. The vertex type
     * however ain't known in receiveDanglingMessages(). So I just removed this
     * code fragment and hope that the code does not run into a deadlock.
     */
    virtual void receiveDanglingMessages();

    /**
     * Releases all the messages. Should be called after every iteration. The
     * operation runs through all the buffers and calls release for each of
     * them. First, the sent messages are released, then, the node has to wait
     * until all the required messages for the next iteration are received.
     * Both steps might in turn invoke indirectly receiveDanglingMessages() due
     * to the Node services. Consequently, the implementation can reduce to a
     * simple wait for enough messages - it does not have to actively receive
     * messages (this is done due to receiveDanglingMessages() anyway).
     */
    void releaseMessages();

    /**
     * Sends a message to the destination node. The vertex might be buffered, so
     * no send is triggered immediately. This operation also implements the lazy
     * buffer creation, i.e. if a vertex is sent to a rank for which no buffer
     * exists yet, it creates this buffer. This is a fundamental difference to
     * the corresponding receive operation.
     *
     * As the operation also creates the buffers, it has to know how one will
     * access the buffer, i.e. in a LIFO or FIFO order.
     */
    template <class Vertex>
    void sendVertex( const Vertex& vertex, int toRank, const BufferAccessType& bufferAccessType );

    /**
     * Returns the next element from the receive buffer. Internally there are
     * two receive buffers to make the program able to do asynchronous
     * receives. So the receive buffers are called receive and deploy buffer and
     * this operation returns the elements of the deploy buffer. The deploy
     * buffer is read in a stack manner, i.e. from right to left.
     *
     * You are never allowed to receive a vertex from a rank to which you
     * haven't sent a vertex before.
     *
     * The operation ain't a const operation at the getter might and will
     * trigger reorganisations and state changes of the underlying buffers.
     */
    template <class Vertex>
    Vertex getVertex(int fromRank);

    /**
     * Set a new buffer size.
     */
    void setBufferSize( int bufferSize );
};

#include "peano/parallel/SendReceiveBufferPool.cpph"


#endif