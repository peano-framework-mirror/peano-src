// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_GRID_TESTS_SINGLE_LEVEL_ENUMERATOR_TEST_H_
#define _PEANO_GRID_TESTS_SINGLE_LEVEL_ENUMERATOR_TEST_H_


#include "tarch/tests/TestCase.h"
#include "peano/grid/tests/records/TestCell.h"
#include "peano/grid/Cell.h"


namespace peano {
  namespace grid {
    namespace tests {
      class SingleLevelEnumeratorTest;
    }
  }
}


class peano::grid::tests::SingleLevelEnumeratorTest: public tarch::tests::TestCase {
  private:
    static tarch::logging::Log   _log;

    void test2DLineariseCellIndex();
    void test2DLineariseVertexIndex();
    void testFunctorWithZeroArgument();
    void testGetVertexPositionOnCoarserLevel();
    void test2DFunctor();
  public:
    SingleLevelEnumeratorTest();

    virtual ~SingleLevelEnumeratorTest();

    virtual void run();

    void virtual setUp();
};

#endif
