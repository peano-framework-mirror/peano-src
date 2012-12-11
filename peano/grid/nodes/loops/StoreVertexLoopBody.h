// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_GRID_NODES_LOOPS_STORE_VERTEX_LOOP_BODY_H_
#define _PEANO_GRID_NODES_LOOPS_STORE_VERTEX_LOOP_BODY_H_


#include "tarch/logging/Log.h"
#include "tarch/multicore/BooleanSemaphore.h"
#include "tarch/multicore/MulticoreDefinitions.h"

#include "peano/utils/Globals.h"
#include "peano/datatraversal/Action.h"
#include "peano/geometry/Geometry.h"


namespace peano {
  namespace grid {
    namespace nodes {
      namespace loops {
        template <class Vertex,class Cell,class State,class VertexStack,class EventHandle, int CellFlagsFromEnumerator>
        class StoreVertexLoopBody;
      }
    }
  }
}




/**
 * Store Loop Body
 *
 * !!! Optimisation
 *
 * If there were a hanging node adjacent to the leaf, the vertex enumerators's
 * flag, i.e. CellFlagsFromEnumerator would not be equal to leaf anymore.
 *
 *
 * @author Tobias Weinzierl
 */
template <class Vertex, class Cell, class State, class VertexStack, class EventHandle, int CellFlagsFromEnumerator>
class peano::grid::nodes::loops::StoreVertexLoopBody {
  private:
    static tarch::logging::Log _log;

    static tarch::multicore::BooleanSemaphore  _semaphore;

    State&                                    _state;
    Vertex*                                   _fineGridVertices;
    const SingleLevelEnumerator&              _fineGridVerticesEnumerator;
    Cell&                                     _coarseGridCell;
    Vertex*                                   _coarseGridVertices;
    const SingleLevelEnumerator&              _coarseGridVerticesEnumerator;
    const tarch::la::Vector<DIMENSIONS,int>&  _fineGridPositionOfCell;
    int*                                      _counter;
    VertexStack&                              _vertexStack;

#if defined(SharedMemoryParallelisation)
    EventHandle&                                                _eventHandle;
    EventHandle                                                 _threadLocalEventHandle;
#else
    EventHandle&                                                _eventHandle;
    EventHandle&                                                _threadLocalEventHandle;
#endif

    /**
     * Usually, the geometry should be const. See getters of geometry for a
     * description.
     */
    peano::geometry::Geometry&                _geometry;

    /**
     * Holds if and only if an immediate refine has been triggered.
     */
    bool   _hasRefined;
    bool   _hasErased;
    bool   _hasTriggeredRefinementForNextIteration;
    bool   _hasTriggeredEraseForNextIteration;

    const bool   _runsInParallel;

    /**
     * Update Refinement Status Before Data is Written to Output Stream
     *
     * t.b.d.
     *
     * !!! Thread-safety
     *
     * This operation is invoked for vertices fetched from the input stack.
     * The access to the output stack is already serialised, i.e. there's
     * never two simultaneous accesses to the output stack. Consequently, this
     * operation also is serialised and there's no need to make it thread-safe
     * explicitly.
     *
     * !!! Parallelisation
     *
     * If the vertex is a boundary vertex, we send away copies to the neighbour
     * partitions. If it is remote, i.e. not adjacent to the current domain, no
     * sends are triggered. Instead, we erase the vertex immediately.
     */
    void updateRefinementFlagsAndStoreVertexToOutputStream(int positionInArray, const tarch::la::Vector<DIMENSIONS,int>& positionInLocalCell);

    /**
     * Exchange and update parallel information
     *
     * The operation degrades to nop if the code is not compiled with MPI
     * support. We call this operation for each persistent vertex before
     * it is streamed to the output stack, i.e. before
     * updateRefinementFlagsAndStoreVertexToOutputStream() is called.
     * The operation also validates that the vertex is adjacent to a remote
     * rank. If it is not, it immediately returns. Otherwise, it runs through
     * a sequence of checks.
     *
     * !!! Code is joining with a worker
     *
     * We are in the second phase of the join, i.e. now data is transferred
     * from the worker to this node. As a consequence, we can replace all
     * worker entries by the local rank's number before we write the vertex
     * to the output stream.
     *
     * !!! Code is joining with master
     *
     * We are in the second phase of the joint, i.e. the local data is moved
     * to the master. There is nothing to do on the local rank - all data will
     * be thrown away after this iteration anyway:
     *
     * Usually, non-remote vertices that are adjacent to a neighbour are
     * passed to the send receive buffer pool. Yet, such vertices are
     * ignored if we join with the master. As this is the second step of
     * a join process where actually the whole local tree is transferred
     * to the master, we can be sure that noone has sent us any vertices
     * anymore and nobody is expecting this node to send any information.
     *
     *
     * !!! Code is not joining with master
     *
     * If the vertex is not remote (it holds the local rank number as adjacent
     * number as well), send a copy to the neighbour. If the join with the
     * master is already triggered, we have to replace the entries before.
     * Otherwise, we can take the plain copy but have to pass it to the event
     * handle before.
     *
     * !!! Vertex is not adjacent to local domain
     *
     * In this case, the vertex has not to be sent away. The local rank is not
     * adjacent to it, so it is not involved in any communication for this
     * vertex. If such a vertex is not outside yet, we have deployed it to
     * another rank in this iteration and now are not responsible for this
     * vertex anymore. Consequently, we can call a destroy event for this
     * vertex and coarse it.
     *
     * !!! Remote vertex detected throughout first traversal of new worker
     *
     * If we detect a remote vertex in the first iteration on a worker, we
     * set all adjacency information to the local master: The vertex adjacency
     * now is correct but we will not be informed when it changes later on
     * due to load balancing. So it makes sense, to set all data to the master
     * rank indicating that the master knows more about the state of this
     * vertex while we keep not track of its state.
     *
     * If this check is removed, the following scenario may happen: Node 1
     * forks into 2 and 3 in one step. On node 2, are remote vertex has the
     * adjacent ranks 0 and 3. It is not kept consistent anymore - it does not
     * take part in any communication. Let's now assume that 1 and 3 merge.
     * So fine, everything is fine, but our vertex with 0,3 is not updated to
     * hold exclusively 0s. Unfortunately, our local node 2 accepts
     * 3 as new worker. Now, this (invalid) adjacency information makes
     * problems, as Peano considers a remote vertex suddenly as domain
     * boundary vertex.
     */
    void exchangeVerticesDueToParallelisation(int positionInArray, const tarch::la::Vector<DIMENSIONS,int>& positionInLocalCell);


    void destroyHangingNode(int positionInArray, const tarch::la::Vector<DIMENSIONS,int>& positionInLocalCell);

    /**
     *
     * !!! Parallelisation
     *
     * Shall not be called for remote vertices, i.e. for such vertices the operation becomes nop.
     */
    void destroyPersistentNode(int positionInArray, const tarch::la::Vector<DIMENSIONS,int>& positionInLocalCell);

    /**
     *
     * !!! Parallelisation
     *
     * Shall not be called for remote vertices, i.e. for such vertices the operation becomes nop.
     */
    void invokeStoreVertexEvent(int positionInArray, const tarch::la::Vector<DIMENSIONS,int>& positionInLocalCell);

    /**
     * In debug mode, this operation compares the vertex's position and level
     * to the cell data.
     */
    void validateVertexInArray( int positionInVertexArray, const tarch::la::Vector<DIMENSIONS,int>& positionInLocalCell ) const;

    /**
     * Restrict the Information on Static Subtrees to the Coarser Levels
     *
     * !!! Thread-safety
     *
     * Should be thread safe, as writes to the output stream are synchronised anyway.
     */
    void updateCoarseGridTreeHeightAttributes(
      int                                       positionInVertexArray,
      const tarch::la::Vector<DIMENSIONS,int>&  positionInLocalCell
    );

    /**
     * Invalidate Coarse Grid Entries
     *
     * Should be called only for (persistent) vertices written to the temporary
     * stacks.
     *
     * Usually, I restrict regularity information (where are regular blocks)
     * solely for vertices written to the output stream. The story however is
     * different, if we do an on-the-fly refinement, i.e. if the user calls
     * refine() in touchVertexFirstTime() and the code decides to refine
     * immediately.
     *
     * @image html StoreVertexLoopBody.png
     *
     * In the example above let the grey block be instationary, the blue one is
     * a regular tree. Grey is visited before blue. Within the grey block, the
     * code decides to refine the red vertex throughout the
     * touchVertexFirstTime(). The traversal automaton does the refinement
     * immediately. Afterwards, they bluegrey grid does exist. The red vertex is
     * written to the temporary stack for the blue adjacent domain.
     *
     * However, the code still assumes that blue is stationary, as the flags
     * have not been invalidated. For this reason, this operation immediately
     * invalidates all coarse grid flags, if an immediate-refine happens. If it
     * were a refinement process split up into two iterations, i.e. into
     * refinement-triggered and refining, this problem would not occur.
     *
     * To make this operation work recursively, i.e. among several levels, it
     * is important that not only refining vertices make their coarse grid
     * vertices invalid. Also invalidated vertices have in turn to invalidate
     * their coarse grid vertices.
     */
    void invalidateCoarseGridTreeHeightAttributesIfRefined(
      int                                       positionInVertexArray,
      const tarch::la::Vector<DIMENSIONS,int>&  positionInLocalCell
    );
  public:
    StoreVertexLoopBody(
      State&                                    state,
      Vertex*                                   fineGridVertices,
      const SingleLevelEnumerator&              fineGridVerticesEnumerator,
      Cell&                                     coarseGridCell,
      Vertex*                                   coarseGridVertices,
      const SingleLevelEnumerator&              coarseGridVerticesEnumerator,
      const tarch::la::Vector<DIMENSIONS,int>&  fineGridPositionOfCell,
      int*                                      counter,
      VertexStack&                              vertexStack,
      EventHandle&                              eventHandle,
      peano::geometry::Geometry&                geometry,
      bool                                      runsInParallel
    );

    ~StoreVertexLoopBody();

    /**
     *
     *
     *
|| Stack \\ Counter || persistent                        || hanging                           || new                               || delete                            || none
|| InOut            |  store(-1)                         |  destroy hanging node              |  store(-1)                         |  destroy persistent vertex         |  nop (vertex is still needed)
|| Temporary        |  store(stack number)               |  store(stack number)               |  store(stack number)               |  store(stack number)               |  nop (vertex is still needed)
     */
    void operator() (const peano::datatraversal::Action& action);
};


#include "peano/grid/nodes/loops/StoreVertexLoopBody.cpph"


#endif
