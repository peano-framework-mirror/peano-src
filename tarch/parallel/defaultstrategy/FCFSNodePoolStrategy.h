// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#if !defined(_TARCH_PARALLEL_FCFS_NODE_POOL_STRATEGY_H_) && defined(Parallel)
#define _TARCH_PARALLEL_FCFS_NODE_POOL_STRATEGY_H_

#ifdef Parallel
#include <mpi.h>
#endif

#include "tarch/parallel/NodePoolStrategy.h"
#include "tarch/parallel/NodePoolListEntry.h"
#include "tarch/logging/Log.h"

#include <list>


namespace tarch {
  namespace parallel {
    class FCFSNodePoolStrategy;
  }
}


/**
 * Node Pool Strategy: Fair
 *
 * This is a very simple node pool strategy. It was written to enable Peano
 * users to come up with a running parallel prototype fast, but it is not
 * tuned to any pde-specific properties. It thus makes sense to implement
 * NodePoolStrategy independently for a given project.
 *
 * @author Tobias Weinzierl
 * @version $Revision: 1.6 $
 */
class tarch::parallel::FCFSNodePoolStrategy: public tarch::parallel::NodePoolStrategy {
  private:

    typedef std::list<NodePoolListEntry>   NodeContainer;

    /**
     * Logging Device
     */
    static tarch::logging::Log _log;

    /**
     * Tag on which the node pool works
     */
    int _tag;

    /**
     * The ist the list of active nodes. Every entry corresponds to one node.
     * If the entry is set, the node is working already and the server is not
     * allowed to deploy another job on this node. If the entry isn't set, there
     * is a job request message in the queue and the server is allowed to send
     * a job. Therefore, in the beginning, all the entries are set. For the very
     * first entry, corresponding to the server node, the invariant holds, that
     * this entry is set always.
     */
    NodeContainer _nodes;

    void logQueue( const RequestQueue& queue ) const;

  public:
	/**
	 * Constructor
	 *
	 * Construct all the attributes.
	 */
    FCFSNodePoolStrategy();
    virtual ~FCFSNodePoolStrategy();

    virtual void setNodePoolTag(int tag);
    virtual tarch::parallel::messages::WorkerRequestMessage extractElementFromRequestQueue(RequestQueue& queue);
    virtual void fillWorkerRequestQueue(RequestQueue& queue);
    virtual void addNode(const tarch::parallel::messages::RegisterAtNodePoolMessage& node );
    virtual void removeNode( int rank );
    virtual int getNumberOfIdleNodes() const;
    virtual void setNodeIdle( int rank );
    virtual int reserveNode(int forMaster);
    virtual bool isRegisteredNode(int rank) const;
    virtual bool isIdleNode(int rank) const;
    virtual void clearRegisteredNodes();
    virtual int getNumberOfRegisteredNodes() const;
    virtual std::string toString() const;
    virtual bool hasIdleNode() const;
    virtual int removeNextIdleNode();
};

#endif
