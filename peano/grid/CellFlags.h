// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_GRID_CELL_FLAGS_H_
#define _PEANO_GRID_CELL_FLAGS_H_


#include <string>


namespace peano {
    namespace grid {
      /**
       * Extension of the superclass' cell flags
       */
      enum CellFlags {
        /**
         * Is set by safe and clear.
         */
        Undefined                      = 65536,
        Leaf                           = 0,
        /**
         * Only assigned to hanging nodes (see default constructor).
         */
        StationaryButIrregular          = -1,
        StationaryWithParallelBoundary  = -2,
        NotStationary                   = -3,
        /**
         * Basically, this should be NotStationary, but I prefer to see in the
         * visualisation where the not-stationary flag comes from.
         */
        NotStationaryDueToInvalidation  = -4
      };

      peano::grid::CellFlags min( const peano::grid::CellFlags& lhs, const peano::grid::CellFlags& rhs);

      void inc( peano::grid::CellFlags& value);
  }
}


/**
 * Gives you a readable representation of the cell flags.
 */
std::string toString( const peano::grid::CellFlags& cellFlags );

#endif
