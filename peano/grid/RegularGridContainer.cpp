#include "peano/grid/RegularGridContainer.h"


bool peano::grid::mayForkLoadOrStoreVertexTaskOnRegularSubtree(
  const int   currentLevel,
  const bool  isParentCellAtPatchBoundary,
  const int   maxLevelToFork,
  const int   coarsestLevelOfThisTask,
  const int   expectedNumberOfLoadsOrStores
) {
  return !isParentCellAtPatchBoundary &&
         expectedNumberOfLoadsOrStores>0 &&
         maxLevelToFork>=currentLevel &&
         currentLevel>coarsestLevelOfThisTask;
}

