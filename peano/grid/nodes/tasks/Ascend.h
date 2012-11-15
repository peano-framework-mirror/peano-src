// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_GRID_NODES_TASKS_ASCEND_H_
#define _PEANO_GRID_NODES_TASKS_ASCEND_H_


#include "peano/utils/Globals.h"

#include "peano/grid/nodes/Node.h"
#include "peano/grid/RegularGridContainer.h"

#include "peano/datatraversal/autotuning/OracleForOnePhase.h"

#include "peano/grid/nodes/loops/CallLeaveCellLoopBodyOnRegularRefinedPatch.h"
#include "peano/grid/nodes/loops/CallTouchVertexLastTimeLoopBodyOnRegularRefinedPatch.h"


namespace peano {
  namespace grid {
    namespace nodes {
      namespace tasks {
        template <class Vertex, class Cell, class State, class EventHandle>
        class Ascend;
      }
    }
  }
}




/**
 * Ascend on regular refined subtree
 */
template <class Vertex, class Cell, class State, class EventHandle>
class peano::grid::nodes::tasks::Ascend {
  private:
    typedef peano::grid::RegularGridContainer<Vertex,Cell>                                                             RegularGridContainer;
    typedef peano::grid::nodes::loops::CallLeaveCellLoopBodyOnRegularRefinedPatch<Vertex,Cell,EventHandle>             LeaveCellLoopBody;
    typedef peano::grid::nodes::loops::CallTouchVertexLastTimeLoopBodyOnRegularRefinedPatch<Vertex,Cell,EventHandle>   TouchVertexLastTimeLoopBody;

    const int              _treeDepth;
    State&                 _state;
    EventHandle&           _eventHandle;
    RegularGridContainer&  _gridContainer;
    bool                   _treeRemainsStatic;

    TouchVertexLastTimeLoopBody  _touchVertexLastTimeLoopBody;
    LeaveCellLoopBody            _leaveCellLoopBody;

    void touchVerticesLastTime(int level);
    void ascend(int fineGridLevel);
    void leaveCells(int level);
  public:
    Ascend(
      const int              treeDepth,
      State&                 state,
      EventHandle&           eventHandle,
      RegularGridContainer&  gridContainer
    );

    void operator() ();
};


#include "peano/grid/nodes/tasks/Ascend.cpph"

#endif

