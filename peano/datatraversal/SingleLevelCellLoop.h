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
 * Realises a @f$ 2^d @f$ red-black Gauss-Seidel-type traversal of the
 * Cartesian grid. Its realisation follows dForLoop.
 *
 * @author Tobias Weinzierl
 */

template <class LoopBody>
class peano::datatraversal::SingleLevelCellLoop {
  private:
    void runSequentially(
      LoopBody&                                 loopBody,
      const tarch::la::Vector<DIMENSIONS,int>&  range
    );

    void runParallel(
      LoopBody&                                 loopBody,
      const tarch::la::Vector<DIMENSIONS,int>&  range,
      int                                       grainSize
    );

    /**
      * @see ActionSetTraversalLoop::ActionSetTraversalLoopWithBiPartitioning
      */
     class dForLoopWithBipartitioning {
       private:
         LoopBody     _loopBody;
         const bool   _isLeftTask;
         dForRange    _range;
         const tarch::la::Vector<DIMENSIONS,int>  _offset;
      public:
         dForLoopWithBipartitioning( const LoopBody& loopBody, const tarch::la::Vector<DIMENSIONS,int>& offset, bool isLeftTask, dForRange range );

         /**
          * Process range
          */
         void operator() ();
     };

     class dForLoopInstance {
       private:
         LoopBody                                 _loopBody;
         const tarch::la::Vector<DIMENSIONS,int>  _offset;

       public:
         dForLoopInstance( const LoopBody& loopBody, const tarch::la::Vector<DIMENSIONS,int>& offset );

         #if defined(SharedTBB)
         typedef tbb::split   SplitFlag;
         #else
         typedef int          SplitFlag;
         #endif

         /**
          * Copy Constructor
          *
          * TBB requires the copy constructor for loops to accept an additional
          * argument to be able to distinguish it from the standard copy
          * constructor. As a consequence, the code does not compile anymore
          * without tbb. I thus wrap the TBB type with a typedef of my own.
          */
         dForLoopInstance( const dForLoopInstance& loopBody, SplitFlag );

         /**
          * Process range
          *
          * Could, at first glance, be const as the method copies the loop body anyway. The
          * operation first copies the loop body. This process can run
          * in parallel, as the copy process may not modify the original
          * loop body instance. When the operation has terminated, it calls the
          * loop body copy's updateGlobalValues(). Usually, the copy holds a
          * reference to the original data. A reference not used until this
          * final operation is called. The final operation then commits changes
          * to the original data set. This operation hence is not const.
          * Consequently, the whole operator may not be const.
          */
         void operator() (const dForRange& range);

         /**
          * Empty Join
          *
          * Instead, we use the destructor. See remarks on TBB realisation for
          * encapsulating class.
          */
         void join(const dForLoopInstance&  with);
     };

  public:
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
