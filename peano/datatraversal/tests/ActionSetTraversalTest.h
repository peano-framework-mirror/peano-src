// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_DATA_TRAVERSAL_TESTS_ACTION_SET_TRAVERSAL_TEST_H_
#define _PEANO_DATA_TRAVERSAL_TESTS_ACTION_SET_TRAVERSAL_TEST_H_


#include "tarch/tests/TestCase.h"


namespace peano {
    namespace datatraversal {
      namespace tests {
        class ActionSetTraversalTest;
      }
    }
}


/**
 *
 * @author Tobias Weinzierl
 */
class peano::datatraversal::tests::ActionSetTraversalTest: public tarch::tests::TestCase {
  private:
    void testSequentialCartesianGridTraversalOn10x10Grid();
    void testParallelCartesianGridTraversalOn2D10x10Grid();
  public:
    ActionSetTraversalTest();
    virtual ~ActionSetTraversalTest();

    virtual void run();
    virtual void setUp();
};


#endif
