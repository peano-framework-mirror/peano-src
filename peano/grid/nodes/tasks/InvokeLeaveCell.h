// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_GRID_NODES_TASKS_INVOKE_LEAVE_CELL_H_
#define _PEANO_GRID_NODES_TASKS_INVOKE_LEAVE_CELL_H_


#include "tarch/logging/Log.h"
#include "tarch/multicore/BooleanSemaphore.h"
#include "tarch/multicore/MulticoreDefinitions.h"

#include "peano/utils/Globals.h"
#include "peano/kernel/datatraversal/TaskSet.h"
#include "peano/kernel/spacetreegrid/SingleLevelEnumerator.h"


namespace peano {
    namespace grid {
      namespace nodes {
        namespace tasks {
          template <
            class Vertex,
            class Cell,
            class State,
            class EventHandle
          >
          class InvokeLeaveCell;
        }
      }
    }
}




template < class Vertex, class Cell, class State, class EventHandle
>
class peano::spacetreegrid::nodes::tasks::InvokeLeaveCell {
  private:
    static tarch::logging::Log _log;

    const State&                                                _state;
    Cell&                                                       _fineGridCell;
    Vertex* const                                               _fineGridVertices;
    const peano::spacetreegrid::SingleLevelEnumerator&  _fineGridVerticesEnumerator;
    Cell&                                                       _coarseGridCell;
    Vertex*                                                     _coarseGridVertices;
    const SingleLevelEnumerator&                                _coarseGridVerticesEnumerator;
    const tarch::la::Vector<DIMENSIONS,int>&                    _fineGridPositionOfCell;

#if defined(SharedMemoryParallelisation)
    EventHandle&                                                _eventHandle;
    EventHandle                                                 _threadLocalEventHandle;
#else
    EventHandle&                                                _eventHandle;
    EventHandle&                                                _threadLocalEventHandle;
#endif

  public:
    InvokeLeaveCell(
      State&                                                      state,
      Cell&                                                       fineGridCell,
      Vertex* const                                               fineGridVertices,
      const peano::spacetreegrid::SingleLevelEnumerator&  fineGridVerticesEnumerator,
      Cell&                                                       coarseGridCell,
      Vertex* const                                               coarseGridVertices,
      const peano::spacetreegrid::SingleLevelEnumerator&  coarseGridVerticesEnumerator,
      const tarch::la::Vector<DIMENSIONS,int>&                    fineGridPositionOfCell,
      EventHandle&                                                eventHandle
    );

    ~InvokeLeaveCell();

    void operator()();
};


#include "peano/kernel/spacetreegrid/nodes/tasks/InvokeLeaveCell.cpph"


#endif
