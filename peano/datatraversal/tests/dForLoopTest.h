// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef PEANO_DATATRAVERSAL_TESTS_DFORLOOPTEST_H_
#define PEANO_DATATRAVERSAL_TESTS_DFORLOOPTEST_H_

#include "peano/utils/Globals.h"
#include "tarch/tests/TestCase.h"
#include "tarch/la/Vector.h"
#include "tarch/logging/Log.h"
#include "tarch/multicore/BooleanSemaphore.h"

namespace peano {
    namespace datatraversal {
      namespace tests {
        class dForLoopTest;
      }
    }
}

namespace peano
{
    namespace datatraversal
    {
      namespace tests
      {
        class dForLoopTest : public tarch::tests::TestCase
        {
          private:
          /**
           * Logging device
           */
            static tarch::logging::Log _log;

          /**
           * This test divides a 4x4 patch of vertices into the appropriate ranges, using a grain size of 1.
           * Thus, the result should be to have 16 ranges, each holding one vertex.
           */
            void testCreateRangesVectorGrainSize1();

            /**
             * This test divides a 4x4 patch of vertices into the ranges while putting all boundary
             * vertices into one range. Thus, the result should be to have 5 ranges, one holding
             * the boundary vertices and four for the four inner vertices.
             */
            void testCreateRangesVectorWithSequentialBoundaryGrainSize1();

            /**
             * This test divides a 12x12 patch of vertices into the appropriate ranges
             * while putting all boundary vertices into one range. A grain size of 4 is
             * used. Thus, the expected result is to have one boundary range and 32 ranges
             * holding the rest of the vertices.
             */
            void testCreateRangesVectorWithSequentialBoundary12x12GrainSize4();

            /**
             * This test checks wether the reduction at the end of a parallel for is performed
             * correctly.
             */
            void testParallelReduction();

          public:
            dForLoopTest();
            virtual ~dForLoopTest();

            virtual void run();
            virtual void setUp();
        };

        class TestLoopBody {
          public:
            int _counter;

            static int _globalCounter;

            static tarch::multicore::BooleanSemaphore _semaphore;

            TestLoopBody();
            TestLoopBody(const TestLoopBody& copy);
            ~TestLoopBody();
            void operator() (const tarch::la::Vector<DIMENSIONS,int>& i);

            static void resetGlobalCounter();
        };
      }
    }
}

#endif
