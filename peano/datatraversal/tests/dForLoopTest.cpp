#include "peano/datatraversal/tests/dForLoopTest.h"

#include "peano/datatraversal/dForLoop.h"

#include "tarch/tests/TestCaseFactory.h"
registerTest(peano::datatraversal::tests::dForLoopTest)


tarch::logging::Log peano::datatraversal::tests::dForLoopTest::_log("peano::datatraversal::tests::dForLoopTest");

#ifdef UseTestSpecificCompilerSettings
#pragma optimize("",off)
#endif

peano::datatraversal::tests::dForLoopTest::dForLoopTest()
{
}

peano::datatraversal::tests::dForLoopTest::~dForLoopTest()
{
}

void peano::datatraversal::tests::dForLoopTest::run()
{
  testMethod( testCreateRangesVectorGrainSize1 );
  testMethod( testCreateRangesVectorWithSequentialBoundaryGrainSize1 );
  testMethod( testCreateRangesVectorWithSequentialBoundary12x12GrainSize4 );
  testMethod( testParallelReduction );
}


void peano::datatraversal::tests::dForLoopTest::setUp()
{
}

void peano::datatraversal::tests::dForLoopTest::testCreateRangesVectorGrainSize1()
{
  #ifdef Dim2
  tarch::la::Vector<DIMENSIONS,int> range(4);
  int grainSize = 1;
  bool sequentialBoundary = false;

  TestLoopBody testLoopBody;
  peano::datatraversal::dForLoop<TestLoopBody> loop(range, testLoopBody, grainSize, sequentialBoundary);

  std::vector<peano::datatraversal::dForRange> ranges = loop.createRangesVector(range, grainSize, sequentialBoundary);

  validateEquals(16, ranges.size());

  for(int i = 0; i < static_cast<int>(ranges.size()); i++) {
    for(int d = 0; d < DIMENSIONS; d++){
      validateEquals(1, ranges[i].getRange()(d));
    }
  }

  validateEquals(3, ranges[0].getOffset()(0))
  validateEquals(3, ranges[0].getOffset()(1))
  validateEquals(1, ranges[1].getOffset()(0))
  validateEquals(3, ranges[1].getOffset()(1))
  validateEquals(3, ranges[2].getOffset()(0))
  validateEquals(1, ranges[2].getOffset()(1))
  validateEquals(1, ranges[3].getOffset()(0))
  validateEquals(1, ranges[3].getOffset()(1))
  validateEquals(2, ranges[4].getOffset()(0))
  validateEquals(3, ranges[4].getOffset()(1))
  validateEquals(0, ranges[5].getOffset()(0))
  validateEquals(3, ranges[5].getOffset()(1))
  validateEquals(2, ranges[6].getOffset()(0))
  validateEquals(1, ranges[6].getOffset()(1))
  validateEquals(0, ranges[7].getOffset()(0))
  validateEquals(1, ranges[7].getOffset()(1))
  validateEquals(3, ranges[8].getOffset()(0))
  validateEquals(2, ranges[8].getOffset()(1))
  validateEquals(1, ranges[9].getOffset()(0))
  validateEquals(2, ranges[9].getOffset()(1))
  validateEquals(3, ranges[10].getOffset()(0))
  validateEquals(0, ranges[10].getOffset()(1))
  validateEquals(1, ranges[11].getOffset()(0))
  validateEquals(0, ranges[11].getOffset()(1))
  validateEquals(2, ranges[12].getOffset()(0))
  validateEquals(2, ranges[12].getOffset()(1))
  validateEquals(0, ranges[13].getOffset()(0))
  validateEquals(2, ranges[13].getOffset()(1))
  validateEquals(2, ranges[14].getOffset()(0))
  validateEquals(0, ranges[14].getOffset()(1))
  validateEquals(0, ranges[15].getOffset()(0))
  validateEquals(0, ranges[15].getOffset()(1))
  #endif
}

void peano::datatraversal::tests::dForLoopTest::testCreateRangesVectorWithSequentialBoundaryGrainSize1()
{
  #ifdef Dim2
  tarch::la::Vector<DIMENSIONS,int> range(4);
  int grainSize = 1;
  bool sequentialBoundary = true;

  TestLoopBody testLoopBody;
  peano::datatraversal::dForLoop<TestLoopBody> loop(range, testLoopBody, grainSize, sequentialBoundary);

  std::vector<peano::datatraversal::dForRange> ranges = loop.createRangesVector(range, grainSize, sequentialBoundary);

  validateEqualsWithParams1(5, ranges.size(), ranges.size());

  int volume = 0 ;
  for(int i = 0; i < static_cast<int>(ranges.size()); i++){
    if(!ranges[i].isSequentialBoundaryRange()) {
      volume += tarch::la::volume(ranges[i].getRange());
    }
  }
  validateEqualsWithParams1(4, volume, volume);
  #endif
}


void peano::datatraversal::tests::dForLoopTest::testCreateRangesVectorWithSequentialBoundary12x12GrainSize4()
{
  #ifdef Dim2
  tarch::la::Vector<DIMENSIONS,int> range(12);
  int grainSize = 4;
  bool sequentialBoundary = true;

  TestLoopBody testLoopBody;
  peano::datatraversal::dForLoop<TestLoopBody> loop(range, testLoopBody, grainSize, sequentialBoundary);

  std::vector<peano::datatraversal::dForRange> ranges = loop.createRangesVector(range, grainSize, sequentialBoundary);

  validateEquals(33, ranges.size());

  //Test total volume
  int volume = 0 ;
  for(int i = 0; i < static_cast<int>(ranges.size()); i++){
    if(!ranges[i].isSequentialBoundaryRange()) {
      volume += tarch::la::volume(ranges[i].getRange());
    } else {
      validateEquals(12*12, tarch::la::volume(ranges[i].getRange()));
    }
  }
  validateEqualsWithParams1(100, volume, volume);


  //Test overlapping
  for(int i = 0; i < static_cast<int>(ranges.size()); i++) {
    for(int j = i+1; j < static_cast<int>(ranges.size()); j++) {
      if(!ranges[i].isSequentialBoundaryRange() && !ranges[j].isSequentialBoundaryRange()) {
        bool foundNonOverlappingInterval = false;
        for(int d = 0; d < DIMENSIONS; d++){
            foundNonOverlappingInterval |= (ranges[i].getOffset()(d) + ranges[i].getRange()(d) <= ranges[j].getOffset()(d) || ranges[j].getOffset()(d) + ranges[j].getRange()(d) <= ranges[i].getOffset()(d));
          }
        validateWithParams4(foundNonOverlappingInterval, ranges[i].getOffset(), ranges[i].getRange(), ranges[j].getOffset(), ranges[j].getRange());
      }
    }
  }
  #endif
}

void peano::datatraversal::tests::dForLoopTest::testParallelReduction() {
  #if defined(Dim2)  && (defined(SharedTBB) || defined(SharedOMP))
  tarch::la::Vector<DIMENSIONS,int> range;
  range(0) = 4;
  range(1) = 4;
  int grainSize = 4;
  bool sequentialBoundary = false;

  TestLoopBody testLoopBody;
  TestLoopBody::resetGlobalCounter();
  peano::datatraversal::dForLoop<TestLoopBody> loop(range, testLoopBody, grainSize, sequentialBoundary);

  std::vector<peano::datatraversal::dForRange> ranges = loop.createRangesVector(range, grainSize, sequentialBoundary);

  validateEquals(ranges.size(), 4);

  validateWithParams1( !ranges[0].empty(),     ranges[0].toString());
  validateEqualsWithParams1( ranges[0].getOffset()(0), 2, ranges[0].toString());
  validateEqualsWithParams1( ranges[0].getOffset()(1), 2, ranges[0].toString());
  validateEqualsWithParams1( ranges[0].getRange()(0),  2, ranges[0].toString());
  validateEqualsWithParams1( ranges[0].getRange()(1),  2, ranges[0].toString());

  validateWithParams1( !ranges[1].empty(),ranges[1].toString());
  validateEqualsWithParams1( ranges[1].getOffset()(0), 0, ranges[1].toString());
  validateEqualsWithParams1( ranges[1].getOffset()(1), 2, ranges[1].toString());
  validateEqualsWithParams1( ranges[1].getRange()(0),  2, ranges[1].toString());
  validateEqualsWithParams1( ranges[1].getRange()(1),  2, ranges[1].toString());

  validateWithParams1( !ranges[2].empty(),ranges[2].toString());
  validateEqualsWithParams1( ranges[2].getOffset()(0), 2, ranges[2].toString());
  validateEqualsWithParams1( ranges[2].getOffset()(1), 0, ranges[2].toString());
  validateEqualsWithParams1( ranges[2].getRange()(0),  2, ranges[2].toString());
  validateEqualsWithParams1( ranges[2].getRange()(1),  2, ranges[2].toString());

  validateWithParams1( !ranges[3].empty(),ranges[3].toString());
  validateEqualsWithParams1( ranges[3].getOffset()(0), 0, ranges[3].toString());
  validateEqualsWithParams1( ranges[3].getOffset()(1), 0, ranges[3].toString());
  validateEqualsWithParams1( ranges[3].getRange()(0),  2, ranges[3].toString());
  validateEqualsWithParams1( ranges[3].getRange()(1),  2, ranges[3].toString());


  validateEquals(TestLoopBody::_globalCounter, 16);

  #endif
}




//TestLoopBody
int peano::datatraversal::tests::TestLoopBody::_globalCounter;
tarch::multicore::BooleanSemaphore peano::datatraversal::tests::TestLoopBody::_semaphore;

peano::datatraversal::tests::TestLoopBody::TestLoopBody():
  _counter(0) {
}


peano::datatraversal::tests::TestLoopBody::TestLoopBody(const TestLoopBody& copy):
  _counter(0) {
}


peano::datatraversal::tests::TestLoopBody::~TestLoopBody() {
  tarch::multicore::Lock lock(_semaphore);

  _globalCounter += _counter;
}

void peano::datatraversal::tests::TestLoopBody::operator()(const tarch::la::Vector<DIMENSIONS,int>& i)
{
  _counter++;
}

void peano::datatraversal::tests::TestLoopBody::resetGlobalCounter() {
  _globalCounter = 0;
}

#ifdef UseTestSpecificCompilerSettings
#pragma optimize("",on)
#endif
