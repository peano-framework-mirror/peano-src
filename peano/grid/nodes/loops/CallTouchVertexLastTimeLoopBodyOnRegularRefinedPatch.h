// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_GRID_NODES_LOOPS_CALL_TOUCHE_VERTEX_LAST_TIME_ON_REGULAR_REFINED_PATCH_H_
#define _PEANO_GRID_NODES_LOOPS_CALL_TOUCHE_VERTEX_LAST_TIME_ON_REGULAR_REFINED_PATCH_H_


#include "tarch/logging/Log.h"
#include "tarch/multicore/BooleanSemaphore.h"
#include "tarch/multicore/MulticoreDefinitions.h"

#include "peano/utils/Globals.h"

#include "peano/datatraversal/Action.h"
#include "peano/grid/RegularGridContainer.h"


namespace peano {
  namespace grid {
    namespace nodes {
      namespace loops {
        template <class Vertex, class Cell, class EventHandle>
        class CallTouchVertexLastTimeLoopBodyOnRegularRefinedPatch;
      }
    }
  }
}




/**
 * Load Loop Body
 *
 * This loop body is responsible to fetch data from the vertex containers and
 * write them to the Cartesian grid data structure. See Grid::loadVertices()
 * for a detailed description of the loop body's semantics.
 *
 * @author Tobias Weinzierl
 */
template <class Vertex, class Cell, class EventHandle>
class peano::grid::nodes::loops::CallTouchVertexLastTimeLoopBodyOnRegularRefinedPatch {
  private:
    static tarch::logging::Log _log;

    int                                        _level;
    const int                                  _treeDepth;

    static tarch::multicore::BooleanSemaphore  _semaphore;

#if defined(SharedMemoryParallelisation)
    EventHandle&                                                _eventHandle;
    EventHandle                                                 _threadLocalEventHandle;
#else
    EventHandle&                                                _eventHandle;
    EventHandle&                                                _threadLocalEventHandle;
#endif

    peano::grid::RegularGridContainer<Vertex,Cell>&  _regularGridContainer;

    /**
     * This field is shared among all threads.
     */
    bool&                                     _treeRemainsStatic;
    bool                                      _localTreeRemainsStatic;

    UnrolledLevelEnumerator  _fineGridEnumerator;
    UnrolledLevelEnumerator  _coarseGridEnumerator;

  public:
    /**
     * Study refinement state of vertex. If the vertex does not change, the
     * operation sets the height of the adjacent
     */
    void performVertexTransition( Vertex& vertex, int level );

    CallTouchVertexLastTimeLoopBodyOnRegularRefinedPatch(
      const int                                        treeDepth,
      EventHandle&                                     eventHandle,
      peano::grid::RegularGridContainer<Vertex,Cell>&  regularGridContainer,
      bool&                                            treeRemainsStatic
    );

    /**
     * Destructor
     *
     * It is important to make the leave cell task handles and this loop to use
     * the same semaphore, as they might run in parallel. Thus, I protect this
     * reduction by the cell's semaphore.
     *
     * @see CallTouchVertexFirstTimeLoopBodyOnRegularRefinedPatch::~CallTouchVertexFirstTimeLoopBodyOnRegularRefinedPatch()
     */
    ~CallTouchVertexLastTimeLoopBodyOnRegularRefinedPatch();

    void setLevel(int value);
    int getLevel() const;

    /**
     * Getter for the local copy of the tree-remains-static-flag to manually copy
     * it back to the global tree-remains-static-flag.
     */
    bool getLocalTreeRemainsStatic() const;

    /**
     * @see RegularRefined::callTouchVertexFirstTime()
     */
    void operator() (const tarch::la::Vector<DIMENSIONS, int>& i);
};


#include "peano/grid/nodes/loops/CallTouchVertexLastTimeLoopBodyOnRegularRefinedPatch.cpph"


#endif
