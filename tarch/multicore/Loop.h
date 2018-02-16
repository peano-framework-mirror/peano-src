// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _TARCH_MULTICORE_LOOP_H_
#define _TARCH_MULTICORE_LOOP_H_


#include "tarch/multicore/dForRange.h"
#include "tarch/la/Vector.h"
#include "tarch/multicore/MulticoreDefinitions.h"

#include <functional>






namespace tarch {
  namespace multicore {
    /**
     * Peano's parallel fors may alter the state (so they are different to TBB
     * where we distinguish fors from reduces). It is up to the user to ensure
     * that nothing bad is happening.
     */
    void parallelFor(
      const tarch::multicore::dForRange<1>&  range,
	  std::function< void( int ) >&          function
    );

    void parallelFor(
      const tarch::multicore::dForRange<1>& range,
      std::function< void( const tarch::la::Vector<1,int>& ) >&  function
    );

    void parallelFor(
      const tarch::multicore::dForRange<2>& range,
	  std::function< void( const tarch::la::Vector<2,int>& ) >&  function
    );

    void parallelFor(
      const tarch::multicore::dForRange<3>& range,
	  std::function< void( const tarch::la::Vector<3,int>& ) >&  function
    );

    void parallelFor(
      const tarch::multicore::dForRange<4>& range,
	  std::function< void( const tarch::la::Vector<4,int>& ) >&  function
    );

    /**
     * Loop over range but ensure that any copy made is merged again
     * into input class. Therefore, the input has to have a functor
     * which accepts an integer vector, and it has to have an operation
     *
     * mergeIntoMasterThread
     *
     * <h2> Serial case </h2>
     *
     * If you compile without TBB or OpenMP, then the parallel reduce is a
     * parallel for basically.
     */
    template <typename F>
    void parallelReduce(
      const tarch::multicore::dForRange<2>&  range,
      F&                                     function
    );

    template <typename F>
    void parallelReduce(
      const tarch::multicore::dForRange<3>&  range,
      F&                                     function
    );

    template <typename F>
    void parallelReduce(
      const tarch::multicore::dForRange<4>&  range,
      F&                                     function
    );
  }
}



#if defined(SharedTBB) || defined(SharedTBBInvade)
#include "tarch/multicore/tbb/Loop.h"
#elif SharedOMP
#include "tarch/multicore/omp/Loop.h"
#else
/**
 * Basically is a for loop that can be parallelised.
 *
 * If you have multicore switched on, the multicore variant replaces the
 * statement with a parallel loop fragment. Please take care that all
 * three arguments have exactly the same type, i.e. avoid to mix integer
 * with signed integer or similar things.
 *
 * !!! Grain size choice
 *
 * The choice of a proper grain size is a delicate art. For most cases, setting
 * the grain size to
 *
 * \code
ProblemSize/tarch::multicore::Core::getInstance().getNumberOfThreads()
\endcode
 *
 * is however a reasonably good value.
 *
 * !!! Catching of variables
 *
 * My default pfor implementations catch the environment via references. Please
 * also take into account that some older gcc versions (<=4.6.1) have a bug and
 * do not capture const variables from outside:
 *
 * http://stackoverflow.com/questions/6529177/capturing-reference-variable-by-copy-in-c0x-lambda
 *
 *
 * @param counter   Name of the counter (identifier).
 * @param from      Start value of counter (integer).
 * @param to        End value of counter (integer).
 * @param minGrainSize Smallest grain size (integer). If the loop is split up
 *                  into subloops, none of them has less than minGrainSize
 *                  entries. In the serial version, this parameter has no
 *                  impact/meaning.
 */
#define pfor(counter,from,to,minGrainSize) \
  for (int counter=from; counter<to; counter++) {


#define endpfor }


template <typename F>
void tarch::multicore::parallelReduce(
  const tarch::multicore::dForRange<2>&  range,
  F&                                     function
) {
  parallelFor(range, function);
}


template <typename F>
void tarch::multicore::parallelReduce(
  const tarch::multicore::dForRange<3>&  range,
  F&                                     function
) {
  parallelFor(range, function);
}


template <typename F>
void tarch::multicore::parallelReduce(
  const tarch::multicore::dForRange<4>&  range,
  F&                                     function
) {
  parallelFor(range, function);
}
#endif


#endif
