#include "peano/datatraversal/tests/ActionSetTraversalTest.h"
#include "peano/datatraversal/ActionSetTraversal.h"
#include "peano/utils/Loop.h"

//#include "tarch/la/Utilities.h"


#include "tarch/tests/TestCaseFactory.h"
registerTest(peano::datatraversal::tests::ActionSetTraversalTest)


#ifdef UseTestSpecificCompilerSettings
#pragma optimize("",off)
#endif


peano::datatraversal::tests::ActionSetTraversalTest::ActionSetTraversalTest():
  TestCase( "peano::datatraversal::tests::ActionSetTraversalTest" ) {
}


peano::datatraversal::tests::ActionSetTraversalTest::~ActionSetTraversalTest() {
}


void peano::datatraversal::tests::ActionSetTraversalTest::run() {
  testMethod( testSequentialCartesianGridTraversalOn10x10Grid );
  testMethod( testParallelCartesianGridTraversalOn2D10x10Grid );
}


void peano::datatraversal::tests::ActionSetTraversalTest::setUp() {
}


void peano::datatraversal::tests::ActionSetTraversalTest::testSequentialCartesianGridTraversalOn10x10Grid() {
  const int numberOfCellsPerDimension = 10;

  tarch::la::Vector<DIMENSIONS,int> sizeOfGrid(numberOfCellsPerDimension);
  peano::datatraversal::ActionSetTraversal testTraversal = peano::datatraversal::ActionSetTraversal::getSequentialCartesianGridTraversal(sizeOfGrid);

  validateEquals( testTraversal.getMaximumPath(), tarch::la::aPowI(DIMENSIONS,numberOfCellsPerDimension) );

  dfor( i, sizeOfGrid) {
    validateEquals( testTraversal.getActionSet( peano::utils::dLinearisedWithoutLookup(i,numberOfCellsPerDimension) ).getNumberOfParallelActions(), 1);
    validateEquals( testTraversal.getActionSet( peano::utils::dLinearisedWithoutLookup(i,numberOfCellsPerDimension) ).getAction(0)._id, 0);
    validateEquals( testTraversal.getActionSet( peano::utils::dLinearisedWithoutLookup(i,numberOfCellsPerDimension) ).getAction(0)._cartesianPosition, i);
  }
}


void peano::datatraversal::tests::ActionSetTraversalTest::testParallelCartesianGridTraversalOn2D10x10Grid() {
  #ifdef Dim2
  const int numberOfCellsPerDimension = 12;
  tarch::la::Vector<DIMENSIONS,int> cellIndex;

  tarch::la::Vector<DIMENSIONS,int> sizeOfGrid(numberOfCellsPerDimension);
  peano::datatraversal::ActionSetTraversal testTraversal = peano::datatraversal::ActionSetTraversal::getParallelCartesianGridTraversal(sizeOfGrid);

  validateEquals( testTraversal.getMaximumPath(),  4);

  validateEquals( testTraversal.getActionSet( 0 ).getNumberOfParallelActions(), tarch::la::aPowI(DIMENSIONS,numberOfCellsPerDimension)/4);
  validateEquals( testTraversal.getActionSet( 1 ).getNumberOfParallelActions(), tarch::la::aPowI(DIMENSIONS,numberOfCellsPerDimension)/4);
  validateEquals( testTraversal.getActionSet( 2 ).getNumberOfParallelActions(), tarch::la::aPowI(DIMENSIONS,numberOfCellsPerDimension)/4);
  validateEquals( testTraversal.getActionSet( 3 ).getNumberOfParallelActions(), tarch::la::aPowI(DIMENSIONS,numberOfCellsPerDimension)/4);

  //Check all actions
  for(int actionSetIndex = 0; actionSetIndex < 4; actionSetIndex++) {
    tarch::la::Vector<DIMENSIONS, int> cellIndex;
    tarch::la::assignList(cellIndex) = (actionSetIndex % 2), (actionSetIndex/2);
    for(int actionIndex = 0; actionIndex < tarch::la::aPowI(DIMENSIONS,numberOfCellsPerDimension)/4; actionIndex++) {
      validateEqualsWithParams2( testTraversal.getActionSet( actionSetIndex ).getAction(actionIndex)._cartesianPosition, cellIndex, actionSetIndex, actionIndex );
      cellIndex(0) += 2;
      if(cellIndex(0) >= numberOfCellsPerDimension) {
        cellIndex(0) = cellIndex(0) % numberOfCellsPerDimension;
        cellIndex(1) += 2;
      }
    }
  }

  //Check some manually
  assignList(cellIndex) = 0,0;
  validateEquals( testTraversal.getActionSet( 0 ).getAction(0)._cartesianPosition, cellIndex );
  assignList(cellIndex) = 2,0;
  validateEquals( testTraversal.getActionSet( 0 ).getAction(1)._cartesianPosition, cellIndex );
  assignList(cellIndex) = 4,0;
  validateEquals( testTraversal.getActionSet( 0 ).getAction(2)._cartesianPosition, cellIndex );
  assignList(cellIndex) = 6,0;
  validateEquals( testTraversal.getActionSet( 0 ).getAction(3)._cartesianPosition, cellIndex );
  assignList(cellIndex) = 8,0;
  validateEquals( testTraversal.getActionSet( 0 ).getAction(4)._cartesianPosition, cellIndex );
  assignList(cellIndex) = 10,0;
  validateEquals( testTraversal.getActionSet( 0 ).getAction(5)._cartesianPosition, cellIndex );
  assignList(cellIndex) = 0,2;
  validateEquals( testTraversal.getActionSet( 0 ).getAction(6)._cartesianPosition, cellIndex );
  assignList(cellIndex) = 2,2;
  validateEquals( testTraversal.getActionSet( 0 ).getAction(7)._cartesianPosition, cellIndex );
  assignList(cellIndex) = 4,2;
  validateEquals( testTraversal.getActionSet( 0 ).getAction(8)._cartesianPosition, cellIndex );

  assignList(cellIndex) = 1,0;
  validateEquals( testTraversal.getActionSet( 1 ).getAction(0)._cartesianPosition, cellIndex );
  assignList(cellIndex) = 3,0;
  validateEquals( testTraversal.getActionSet( 1 ).getAction(1)._cartesianPosition, cellIndex );
  assignList(cellIndex) = 5,0;
  validateEquals( testTraversal.getActionSet( 1 ).getAction(2)._cartesianPosition, cellIndex );
  assignList(cellIndex) = 7,0;
  validateEquals( testTraversal.getActionSet( 1 ).getAction(3)._cartesianPosition, cellIndex );
  assignList(cellIndex) = 9,0;
  validateEquals( testTraversal.getActionSet( 1 ).getAction(4)._cartesianPosition, cellIndex );
  assignList(cellIndex) = 11,0;
  validateEquals( testTraversal.getActionSet( 1 ).getAction(5)._cartesianPosition, cellIndex );
  assignList(cellIndex) = 1,2;
  validateEquals( testTraversal.getActionSet( 1 ).getAction(6)._cartesianPosition, cellIndex );
  assignList(cellIndex) = 3,2;
  validateEquals( testTraversal.getActionSet( 1 ).getAction(7)._cartesianPosition, cellIndex );
  assignList(cellIndex) = 5,2;
  validateEquals( testTraversal.getActionSet( 1 ).getAction(8)._cartesianPosition, cellIndex );


  assignList(cellIndex) = 0,1;
  validateEquals( testTraversal.getActionSet( 2 ).getAction(0)._cartesianPosition, cellIndex );
  assignList(cellIndex) = 2,1;
  validateEquals( testTraversal.getActionSet( 2 ).getAction(1)._cartesianPosition, cellIndex );
  assignList(cellIndex) = 4,1;
  validateEquals( testTraversal.getActionSet( 2 ).getAction(2)._cartesianPosition, cellIndex );
  assignList(cellIndex) = 6,1;
  validateEquals( testTraversal.getActionSet( 2 ).getAction(3)._cartesianPosition, cellIndex );
  assignList(cellIndex) = 8,1;
  validateEquals( testTraversal.getActionSet( 2 ).getAction(4)._cartesianPosition, cellIndex );
  assignList(cellIndex) = 10,1;
  validateEquals( testTraversal.getActionSet( 2 ).getAction(5)._cartesianPosition, cellIndex );
  assignList(cellIndex) = 0,3;
  validateEquals( testTraversal.getActionSet( 2 ).getAction(6)._cartesianPosition, cellIndex );
  assignList(cellIndex) = 2,3;
  validateEquals( testTraversal.getActionSet( 2 ).getAction(7)._cartesianPosition, cellIndex );
  assignList(cellIndex) = 4,3;
  validateEquals( testTraversal.getActionSet( 2 ).getAction(8)._cartesianPosition, cellIndex );

  assignList(cellIndex) = 1,1;
  validateEquals( testTraversal.getActionSet( 3 ).getAction(0)._cartesianPosition, cellIndex );
  assignList(cellIndex) = 3,1;
  validateEquals( testTraversal.getActionSet( 3 ).getAction(1)._cartesianPosition, cellIndex );
  assignList(cellIndex) = 5,1;
  validateEquals( testTraversal.getActionSet( 3 ).getAction(2)._cartesianPosition, cellIndex );
  assignList(cellIndex) = 7,1;
  validateEquals( testTraversal.getActionSet( 3 ).getAction(3)._cartesianPosition, cellIndex );
  assignList(cellIndex) = 9,1;
  validateEquals( testTraversal.getActionSet( 3 ).getAction(4)._cartesianPosition, cellIndex );
  assignList(cellIndex) = 11,1;
  validateEquals( testTraversal.getActionSet( 3 ).getAction(5)._cartesianPosition, cellIndex );
  assignList(cellIndex) = 1,3;
  validateEquals( testTraversal.getActionSet( 3 ).getAction(6)._cartesianPosition, cellIndex );
  assignList(cellIndex) = 3,3;
  validateEquals( testTraversal.getActionSet( 3 ).getAction(7)._cartesianPosition, cellIndex );
  assignList(cellIndex) = 5,3;
  validateEquals( testTraversal.getActionSet( 3 ).getAction(8)._cartesianPosition, cellIndex );
  #endif
}


#ifdef UseTestSpecificCompilerSettings
#pragma optimize("",on)
#endif
