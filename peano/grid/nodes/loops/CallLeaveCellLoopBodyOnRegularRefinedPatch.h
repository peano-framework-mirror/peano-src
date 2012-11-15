// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_GRID_NODES_LOOPS_CALL_LEAVE_CELL_ON_REGULAR_REFINED_PATCH_H_
#define _PEANO_GRID_NODES_LOOPS_CALL_LEAVE_CELL_ON_REGULAR_REFINED_PATCH_H_


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
          class CallLeaveCellLoopBodyOnRegularRefinedPatch;

          /**
           * Forward declaration
           */
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
class peano::spacetreegrid::nodes::loops::CallLeaveCellLoopBodyOnRegularRefinedPatch {
  private:
    static tarch::logging::Log _log;

    static tarch::multicore::BooleanSemaphore  _semaphore;

    int                                        _level;

#if defined(SharedMemoryParallelisation)
    EventHandle&                                                _eventHandle;
    EventHandle                                                 _threadLocalEventHandle;
#else
    EventHandle&                                                _eventHandle;
    EventHandle&                                                _threadLocalEventHandle;
#endif

    peano::spacetreegrid::RegularGridContainer<Vertex,Cell>&  _regularGridContainer;


    UnrolledLevelEnumerator  _fineGridEnumerator;
    UnrolledLevelEnumerator  _coarseGridEnumerator;

  public:
    CallLeaveCellLoopBodyOnRegularRefinedPatch(
      EventHandle&                                                       eventHandle,
      peano::spacetreegrid::RegularGridContainer<Vertex,Cell>&   regularGridContainer
    );

    ~CallLeaveCellLoopBodyOnRegularRefinedPatch();

    void setLevel(int value);
    int getLevel() const;

    /**
     * @see RegularRefined::callTouchVertexFirstTime()
     */
    void operator() (const peano::datatraversal::Action& i);
};


#include "peano/kernel/spacetreegrid/nodes/loops/CallLeaveCellLoopBodyOnRegularRefinedPatch.cpph"


#endif
