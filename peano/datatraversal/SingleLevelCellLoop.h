// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_DATA_TRAVERSAL_SINGLE_LEVEL_CELL_LOOP_H_
#define _PEANO_DATA_TRAVERSAL_SINGLE_LEVEL_CELL_LOOP_H_


#include "peano/datatraversal/ActionSet.h"
#include "peano/datatraversal/ActionSetTraversal.h"


namespace peano {
    namespace datatraversal {
      template <class LoopBody>
      class SingleLevelCellLoop;
    }
}


/**
 * Loop over set of cells on a single level
 *
 * This operation is a wrapper around ActionSetTraversalLoop.
 *
 * @author Tobias Weinzierl
 */

template <class LoopBody>
class peano::datatraversal::SingleLevelCellLoop {
  private:
    void run(
      LoopBody&                                        loopBody,
      int                                              grainSize,
      const peano::datatraversal::ActionSetTraversal&  cellTraversal
    );

//    void run(
//      LoopBody&                                 loopBody,
//      int                                       grainSize,
//      const tarch::la::Vector<DIMENSIONS,int>&  range
//    );
  public:
    /**
     * Shortcut for calling the standard constructor and the run operation
     * afterwards.
     */
    SingleLevelCellLoop(
      const tarch::la::Vector<DIMENSIONS,int>&  range,
      LoopBody&                                 loopBody,
      int                                       grainSize
    );

    /**
     * Standard constructor
     */
    SingleLevelCellLoop();
};



#include "peano/datatraversal/SingleLevelCellLoop.cpph"

#endif
