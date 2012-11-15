#include "peano/datatraversal/autotuning/OracleForOnePhase.h"
#include "tarch/Assertions.h"


std::string peano::datatraversal::autotuning::toString( const MethodTrace& methodTrace ) {
  switch (methodTrace) {
    case RegularGridLoadVertices:
      return "regular-grid-load-vertices";
    case RegularGridStoreVertices:
      return "regular-grid-store-vertices";
    case RegularGridHandleCells:
      return "regular-grid-handle-cells";
    case SpacetreeGridLoadVertices:
      return "spacetree-grid-load-vertices";
    case SpacetreeGridLoadVerticesOnRegularStationaryGrid:
      return "spacetree-grid-load-vertices-on-regular-stationary-grid";
    case SpacetreeGridLoadVerticesOnIrregularStationaryGrid:
      return "spacetree-grid-load-vertices-on-irregular-stationary-grid";
    case SpacetreeGridLoadVerticesOnStationaryGridWithParallelBoundary:
      return "spacetree-grid-load-vertices-on-stationary-grid-with-parallel-boundary";
    case SpacetreeGridStoreVertices:
      return "spacetree-grid-store-vertices";
    case SpacetreeGridStoreVerticesOnIrregularStationaryGrid:
      return "spacetree-grid-store-vertices-on-irregular-stationary-grid";
    case SpacetreeGridStoreVerticesOnRegularStationaryGrid:
      return "spacetree-grid-store-vertices-on-regular-stationary-grid";
    case SpacetreeGridSetCounter:
      return "spacetree-grid-set-counter";
    case SpacetreeGridCallEnterCellAndLoadSubCellsWithinTraverse:
      return "spacetree-grid-enter-cell-and-load-sub-cells";
    case SpacetreeGridCallLeaveCellAndStoreSubCellsWithinTraverse:
      return "spacetree-grid-leave-cell-and-store-sub-cells";
    case SpacetreeGridCallEnterCellAndInitialiseEnumeratorsOnRegularStationaryGrid:
      return "spacetree-grid-call-enter-cell-and-initialise-enumerators-on-regular-stationary-grid";
    case SpacetreeGridCallTouchFirstTimeOnRegularStationaryGrid:
      return "spacetree-grid-call-touch-first-time-on-regular-stationary-grid";
    case SpacetreeGridCallTouchLastTimeOnRegularStationaryGrid:
      return "spacetree-grid-call-touch-last-time-on-regular-stationary-grid";
    case SpacetreeGridCallEnterCellOnRegularStationaryGrid:
      return "spacetree-grid-call-enter-cell-on-regular-stationary-grid";
    case SpacetreeGridCallLeaveCellOnRegularStationaryGrid:
      return "spacetree-grid-call-leave-cell-on-regular-stationary-grid";
    case SpacetreeGridParalleliseAscend:
      return "spacetree-grid-parallelise-ascend";
    case SpacetreeGridParalleliseDescend:
      return "spacetree-grid-parallelise-descend";
    case SpacetreeGridSplitLoadVerticesTaskOnRegularStationaryGrid:
      return "spacetree-grid-split-load-vertices-task-on-regular-stationary-grid";
    case SpacetreeGridSplitStoreVerticesTaskOnRegularStationaryGrid:
      return "spacetree-grid-split-store-vertices-task-on-regular-stationary-grid";
    case NumberOfDifferentMethodsCalling:
      return "<error>";
  }

  return "<error>";
}


peano::datatraversal::autotuning::MethodTrace peano::datatraversal::autotuning::toMethodTrace(int value) {
  assertion( value>=0 );
  assertion( value<NumberOfDifferentMethodsCalling );
  switch (value) {
    case static_cast<int>(RegularGridLoadVertices):
      return RegularGridLoadVertices;
    case static_cast<int>(RegularGridStoreVertices):
      return RegularGridStoreVertices;
    case static_cast<int>(RegularGridHandleCells):
      return RegularGridHandleCells;
    case static_cast<int>(SpacetreeGridLoadVertices):
      return SpacetreeGridLoadVertices;
    case static_cast<int>(SpacetreeGridLoadVerticesOnRegularStationaryGrid):
      return SpacetreeGridLoadVerticesOnRegularStationaryGrid;
    case static_cast<int>(SpacetreeGridLoadVerticesOnIrregularStationaryGrid):
      return SpacetreeGridLoadVerticesOnIrregularStationaryGrid;
    case static_cast<int>(SpacetreeGridLoadVerticesOnStationaryGridWithParallelBoundary):
      return SpacetreeGridLoadVerticesOnStationaryGridWithParallelBoundary;
    case static_cast<int>(SpacetreeGridStoreVertices):
      return SpacetreeGridStoreVertices;
    case static_cast<int>(SpacetreeGridStoreVerticesOnRegularStationaryGrid):
      return SpacetreeGridStoreVerticesOnRegularStationaryGrid;
    case static_cast<int>(SpacetreeGridStoreVerticesOnIrregularStationaryGrid):
      return SpacetreeGridStoreVerticesOnIrregularStationaryGrid;
    case static_cast<int>(SpacetreeGridSetCounter):
      return SpacetreeGridSetCounter;
    case static_cast<int>(SpacetreeGridCallEnterCellAndLoadSubCellsWithinTraverse):
      return SpacetreeGridCallEnterCellAndLoadSubCellsWithinTraverse;
    case static_cast<int>(SpacetreeGridCallLeaveCellAndStoreSubCellsWithinTraverse):
      return SpacetreeGridCallLeaveCellAndStoreSubCellsWithinTraverse;
    case static_cast<int>(SpacetreeGridCallEnterCellAndInitialiseEnumeratorsOnRegularStationaryGrid):
      return SpacetreeGridCallEnterCellAndInitialiseEnumeratorsOnRegularStationaryGrid;
    case static_cast<int>(SpacetreeGridCallTouchFirstTimeOnRegularStationaryGrid):
      return SpacetreeGridCallTouchFirstTimeOnRegularStationaryGrid;
    case static_cast<int>(SpacetreeGridCallTouchLastTimeOnRegularStationaryGrid):
      return SpacetreeGridCallTouchLastTimeOnRegularStationaryGrid;
    case static_cast<int>(SpacetreeGridCallEnterCellOnRegularStationaryGrid):
      return SpacetreeGridCallEnterCellOnRegularStationaryGrid;
    case static_cast<int>(SpacetreeGridCallLeaveCellOnRegularStationaryGrid):
      return SpacetreeGridCallLeaveCellOnRegularStationaryGrid;
    case static_cast<int>(SpacetreeGridParalleliseAscend):
      return SpacetreeGridParalleliseAscend;
    case static_cast<int>(SpacetreeGridParalleliseDescend):
      return SpacetreeGridParalleliseDescend;
    case static_cast<int>(SpacetreeGridSplitLoadVerticesTaskOnRegularStationaryGrid):
      return SpacetreeGridSplitLoadVerticesTaskOnRegularStationaryGrid;
    case static_cast<int>(SpacetreeGridSplitStoreVerticesTaskOnRegularStationaryGrid):
      return SpacetreeGridSplitStoreVerticesTaskOnRegularStationaryGrid;
    case static_cast<int>(NumberOfDifferentMethodsCalling):
      return NumberOfDifferentMethodsCalling;
  }

  return RegularGridLoadVertices;
}
