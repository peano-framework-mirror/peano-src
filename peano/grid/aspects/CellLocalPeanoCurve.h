// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_GRID_ASPECTS_CELL_LOCAL_PEANO_CURVE_H_
#define _PEANO_GRID_ASPECTS_CELL_LOCAL_PEANO_CURVE_H_


#include <bitset>

#include "peano/datatraversal/ActionSetTraversal.h"
#include "peano/utils/PeanoOptimisations.h"
#include "tarch/multicore/MulticoreDefinitions.h"

namespace peano {
    namespace grid {
      namespace aspects {
        class CellLocalPeanoCurve;
      }
      namespace tests {
        class CellLocalPeanoCurveTest;
      }
    }
}


/**
 * Refinement Aspect
 *
 * @author Tobias Weinzierl
 */
class peano::grid::aspects::CellLocalPeanoCurve {
  private:
    /**
     * To access the vertex array of a geometric element you use an integer.
     * This integer corresponds to a bitfield, that indicates for every axis
     * weather the coordinate is 0 or 1 on the unit element.
     */
    typedef std::bitset<DIMENSIONS> Coordinates;

    friend class peano::grid::tests::CellLocalPeanoCurveTest;

    /**
     * This operation is needed if and only if we work with caching
     */
    static CellLocalPeanoCurve& getInstance();

    static Coordinates getFirstVertexIndex( const std::bitset<DIMENSIONS>& evenFlags );

    /**
     * Forwards to createSequentialReadVertexSequence(), then resorts, and
     * finally returns the result.
     */
    static peano::datatraversal::ActionSetTraversal createParallelReadVertexSequence( const tarch::la::Vector<DIMENSIONS_TIMES_TWO,short int>& accessNumber, const std::bitset<DIMENSIONS>& evenFlags, const bool& isTraversalInverted);

    static peano::datatraversal::ActionSetTraversal createSequentialReadVertexSequence( const tarch::la::Vector<DIMENSIONS_TIMES_TWO,short int>& accessNumber, const std::bitset<DIMENSIONS>& evenFlags, const bool& isTraversalInverted);

    static peano::datatraversal::ActionSetTraversal createParallelWriteVertexSequence( const tarch::la::Vector<DIMENSIONS_TIMES_TWO,short int>& accessNumber, const std::bitset<DIMENSIONS>& evenFlags, const bool& isTraversalInverted);

    static peano::datatraversal::ActionSetTraversal createSequentialWriteVertexSequence( const tarch::la::Vector<DIMENSIONS_TIMES_TWO,short int>& accessNumber, const std::bitset<DIMENSIONS>& evenFlags, const bool& isTraversalInverted);

    static int getReadStackIdentifier( const Coordinates& coordinates, const tarch::la::Vector<DIMENSIONS_TIMES_TWO,short int>& accessNumber, const std::bitset<DIMENSIONS>& evenFlags, const bool& isTraversalInverted );

    static int getWriteStackIdentifier( const Coordinates& coordinates, const tarch::la::Vector<DIMENSIONS_TIMES_TWO,short int>& accessNumber, const std::bitset<DIMENSIONS>& evenFlags, const bool& isTraversalInverted );

    static tarch::la::Vector<DIMENSIONS,int> toIntVector(const Coordinates& coordinates);

    /**
     * This is a unique key for each cell-traversal direction combination that
     * is used to cache the action sets.
     */
    static int getUniqueKey( const tarch::la::Vector<DIMENSIONS_TIMES_TWO,short int>& accessNumber, const std::bitset<DIMENSIONS>& evenFlags, bool isTraversalInverted, bool parallel );

    CellLocalPeanoCurve();
  public:
    /**
     * We return static const references, as we internally hold a lookup table
     */
    #if defined(CacheActionSets)
    template <class Cell>
    static const peano::datatraversal::ActionSetTraversal& getReadVertexSequence( const Cell& cell, bool isTraversalInverted, bool parallelIfPossible);
    #else
    template <class Cell>
    static peano::datatraversal::ActionSetTraversal getReadVertexSequence( const Cell& cell, bool isTraversalInverted, bool parallelIfPossible);
    #endif

    /**
     * We return static const references, as we internally hold a lookup table
     */
    #if defined(CacheActionSets)
    template <class Cell>
    static const peano::datatraversal::ActionSetTraversal& getWriteVertexSequence( const Cell& cell, bool isTraversalInverted, bool parallelIfPossible);
    #else
    template <class Cell>
    static peano::datatraversal::ActionSetTraversal getWriteVertexSequence( const Cell& cell, bool isTraversalInverted, bool parallelIfPossible);
    #endif
};


#include "peano/grid/aspects/CellLocalPeanoCurve.cpph"


#endif
