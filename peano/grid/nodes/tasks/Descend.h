// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_GRID_NODES_TASKS_DESCEND_H_
#define _PEANO_GRID_NODES_TASKS_DESCEND_H_


#include "peano/utils/Globals.h"

#include "peano/kernel/spacetreegrid/nodes/Node.h"
#include "peano/kernel/spacetreegrid/RegularGridContainer.h"

#include "peano/kernel/datatraversal/autotuning/OracleForOnePhase.h"

#include "peano/kernel/spacetreegrid/nodes/loops/CallEnterCellLoopBodyOnRegularRefinedPatch.h"
#include "peano/kernel/spacetreegrid/nodes/loops/CallTouchVertexFirstTimeLoopBodyOnRegularRefinedPatch.h"


namespace peano {
    namespace grid {
      namespace nodes {
        namespace tasks {
          template <class Vertex, class Cell, class State, class EventHandle>
          class Descend;
        }
    }
  }
}




/**
 *
 * !!! Rationale
 *
 * This is realised as task but actually it should not be a task: The vertex
 * and cell events are strictly sequential in-between the levels, i.e. these
 * are not tasks that can run in parallel. Only the load and store process can
 * be deployed to an additional thread.
 */
template <class Vertex, class Cell, class State, class EventHandle>
class peano::spacetreegrid::nodes::tasks::Descend {
  private:
    typedef peano::spacetreegrid::RegularGridContainer<Vertex,Cell>                                                                    RegularGridContainer;
    typedef peano::spacetreegrid::nodes::loops::CallEnterCellLoopBodyOnRegularRefinedPatch<Vertex,Cell,State,EventHandle>              EnterCellLoopBody;
    typedef peano::spacetreegrid::nodes::loops::CallTouchVertexFirstTimeLoopBodyOnRegularRefinedPatch<Vertex,Cell,State,EventHandle>   TouchVertexFirstTimeLoopBody;

    const int              _treeDepth;
    State&                 _state;
    EventHandle&           _eventHandle;
    RegularGridContainer&  _gridContainer;

    TouchVertexFirstTimeLoopBody  _touchVertexFirstTimeLoopBody;
    EnterCellLoopBody             _enterCellLoopBody;

    /**
     * Are other tasks running in parallel?
     *
     * Other tasks may run in parallel
     *
     * - if the load vertices, load cells, and descend task may run in parallel
     * - or if the load vertices task may fork.
     *
     * The latter fork relies on asynchronous spawning, i.e. if the load process
     * may fork, we never can be sure without a check that all vertices are
     * loaded even though the main load vertex thread has terminated.
     */
    const bool                   _precedingLoadProcessMayFork;
    const bool                   _descendProcessRunsInParallelToOtherTasks;

    void touchVerticesFirstTime(int level);
    void descend(int fineGridLevel);
    void enterCells(int level);
public:
    Descend(
      const int              treeDepth,
      State&                 state,
      EventHandle&           eventHandle,
      RegularGridContainer&  gridContainer,
      bool                   precedingLoadProcessMayFork,
      bool                   descendProcessRunsInParallelToOtherTasks
    );

    void operator() ();
};


#include "peano/kernel/spacetreegrid/nodes/tasks/Descend.cpph"

#endif

