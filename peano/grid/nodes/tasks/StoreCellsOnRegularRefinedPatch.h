// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_GRID_NODES_TASKS_STORE_CELLS_ON_REGULAR_REFINED_PATCH_H_
#define _PEANO_GRID_NODES_TASKS_STORE_CELLS_ON_REGULAR_REFINED_PATCH_H_


namespace peano {
    namespace grid {
      namespace nodes {
        namespace tasks {
          template <class Vertex, class Cell, class CellStack>
          class StoreCellsOnRegularRefinedPatch;
        }
      }
  }
}


/**
 * I need both the vertex and the cell as template argument as the task fills
 * the grid container with data (only with cells), but the grid container is
 * specified in both vertices and cells.
 *
 * @author Tobias Weinzierl
 */
template <class Vertex, class Cell, class CellStack>
class peano::spacetreegrid::nodes::tasks::StoreCellsOnRegularRefinedPatch {
  private:
    static tarch::logging::Log  _log;

    peano::utils::LoopDirection                                       _topLevelLoopDirection;
    const int                                                         _treeDepth;
    const bool                                                        _isTraversalInverted;
    peano::spacetreegrid::RegularGridContainer<Vertex,Cell>&  _regularGridContainer;
    CellStack&                                                        _cellStack;
    const bool                                                        _storeProcessRunsInParallelToOtherTasks;

    /**
     * Store cells
     *
     * This operation is a recursive operation running through all levels
     * bottom-up. It first waits until all events have been called for a
     * given level. This wait immediately terminates if no events are called
     * in parallel, i.e. if no ascend process is running in parallel.
     *
     * Afterwards, it invokes storedCellOfRegularRefinedGrid(), i.e. it
     * switches the input and output stack statistics (how many vertices are
     * stored/read from which stack) and streams the cell to the output
     * stream. If the vertex push process is running in parallel, we may not
     * invert the statistics on the regular grid data structures, as these
     * properties might be required by the store vertex process. Instead, this
     * process graps a copy of the cells and streams the copy to the output
     * stack.
     */
    void storeCells(
      peano::utils::LoopDirection               loopDirection,
      int                                       currentLevel,
      const tarch::la::Vector<DIMENSIONS,int>&  offsetWithinPatch
    );

  public:
    StoreCellsOnRegularRefinedPatch(
      peano::utils::LoopDirection                                       topLevelLoopDirection,
      const int                                                         treeDepth,
      const bool                                                        isTraversalInverted,
      peano::spacetreegrid::RegularGridContainer<Vertex,Cell>&  regularGridContainer,
      CellStack&                                                        cellStack,
      bool                                                              storeProcessRunsInParallelToOtherTasks
   );

    void operator()();
};


#include "peano/kernel/spacetreegrid/nodes/tasks/StoreCellsOnRegularRefinedPatch.cpph"


#endif
