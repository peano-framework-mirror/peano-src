#include "peano/grid/CellFlags.h"

#include <sstream>


std::string toString( const peano::grid::CellFlags& cellFlags ) {
  switch (cellFlags) {
    case peano::grid::Undefined:
      return "undefined";
    case peano::grid::Leaf:
      return "leaf";
    case peano::grid::StationaryButIrregular:
      return "stationary-but-irregular";
    case peano::grid::StationaryWithParallelBoundary:
      return "stationary-with-parallel-boundary";
    case peano::grid::NotStationary:
      return "not-stationary";
    case peano::grid::NotStationaryDueToInvalidation:
      return "not-stationary-due-to-invalidation";
    default:
      {
        std::ostringstream msg;
        msg << "regular subtree of height " << cellFlags;
        return msg.str();
      }
  }
  return "<error>";
}


peano::grid::CellFlags min( const peano::grid::CellFlags& lhs, const peano::grid::CellFlags& rhs) {
  peano::grid::CellFlags result =
    static_cast<int>(lhs) < static_cast<int>(rhs) ? lhs : rhs;
  return result;
}


void inc( peano::grid::CellFlags& value) {
  value = static_cast<peano::grid::CellFlags>( static_cast<int>(value)+1 );
}
