// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_DATA_TRAVERSAL_ACTION_SET_TRAVERSAL_H_
#define _PEANO_DATA_TRAVERSAL_ACTION_SET_TRAVERSAL_H_


#include "peano/datatraversal/ActionSet.h"
#include "peano/utils/PeanoOptimisations.h"


namespace peano {
    namespace datatraversal {
      class ActionSetTraversal;
    }
}



/**
 * Cell Traversal
 *
 * Represents a complete traversal of a cell data structure. It consists of a
 * sequence of
 *
 * @author Svetlana Nogina, Tobias Weinzierl, Marco Seravalli
 */
class peano::datatraversal::ActionSetTraversal {
  private:
    std::vector< ActionSet >  _actionSets;

    /**
     * Factory method for the action set. If CacheActionSets is unset, the
     * public getter redirects the function call directly to this operation.
     * Otherwise, this operation is the source of the cached return values.
     */
    #if defined(CacheActionSets)
    static ActionSetTraversal*  createParallelCartesianGridTraversal(  const tarch::la::Vector<DIMENSIONS,int>& numberOfCells);
    #else
    static ActionSetTraversal   createParallelCartesianGridTraversal(  const tarch::la::Vector<DIMENSIONS,int>& numberOfCells);
    #endif

    /**
     * Factory method for the action set. If CacheActionSets is unset, the
     * public getter redirects the function call directly to this operation.
     * Otherwise, this operation is the source of the cached return values.
     */
    #if defined(CacheActionSets)
    static ActionSetTraversal*  createSequentialCartesianGridTraversal(const tarch::la::Vector<DIMENSIONS,int>& numberOfCells);
    #else
    static ActionSetTraversal   createSequentialCartesianGridTraversal(const tarch::la::Vector<DIMENSIONS,int>& numberOfCells);
    #endif

    /**
     * Returns key of problem or 0 if there is no key
     */
    static int getKey(const tarch::la::Vector<DIMENSIONS,int>& numberOfCells);

    /**
     * Inverse of getKey().
     */
    static tarch::la::Vector<DIMENSIONS,int> getNumberOfCellsFromKey( int key );
  public:
    /**
     * Standard constructor.
     *
     * Please use static methods to create a cell traversal.
     */
    ActionSetTraversal(int numberOfActionSets);

    /**
     * Destructor
     */
    ~ActionSetTraversal();

    /**
     * Construct parallel cell traversal
     *
     * Construct cell traversal for regular Cartesian grid. This traversal
     * identifies a red-black type colouring with @f$ 2^d @f$ colours, and it
     * consists solely of enter actions. You specify a @f$ numberOfCells_1 \times numberOfCells_2 \times \dots \times numberOfCells_d @f$
     * grid. The operation splits up the elements of this d-dimensional array
     * into @f$ 2^d @f$ different sets such that no two elements within one set
     * are adjacent to each other. And then it returns exactly this sequence of
     * sets that you can execute in parallel.
     *
     * @param numberOfCells Number of cells.
     */
    #if defined(CacheActionSets)
    static const ActionSetTraversal& getParallelCartesianGridTraversal(const tarch::la::Vector<DIMENSIONS,int>& numberOfCells);
    #else
    static ActionSetTraversal getParallelCartesianGridTraversal(const tarch::la::Vector<DIMENSIONS,int>& numberOfCells);
    #endif

    /**
     * This is the sequential version of getSequentialCartesianGridTraversal().
     */
    #if defined(CacheActionSets)
    static const ActionSetTraversal& getSequentialCartesianGridTraversal(const tarch::la::Vector<DIMENSIONS,int>& numberOfCells);
    #else
    static ActionSetTraversal getSequentialCartesianGridTraversal(const tarch::la::Vector<DIMENSIONS,int>& numberOfCells);
    #endif

    /**
     * This operation takes a action set traversal representing a total order,
     * i.e. each action set holds one and only one action. It then assumes that
     * only actions working with the same id cannot run in parallel, and
     * reorders all actions according to this assumption. If there's an action
     * a and an action b with a being before b in input, a will also be before
     * b in the result. Otherwise, they might run in parallel.
     */
    static ActionSetTraversal sortIntoIdBins(const ActionSetTraversal& input);

    /**
     * Return the maximum path.
     *
     * This maximum path also is the number of action sets
     */
    int getMaximumPath() const;

    /**
     * The maximum grain size is the fatest node of the overall sequence, i.e.
     * it identifies the best concurrency level possible.
     */
    int getMaximumGrainSize() const;

    const ActionSet& getActionSet( int i ) const;

    ActionSet& getActionSet( int i );

    std::string toString() const;
};



#endif
