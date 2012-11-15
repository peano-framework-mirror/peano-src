#include "peano/datatraversal/ActionSetTraversal.h"

#include "peano/utils/Loop.h"

#include <vector>


peano::datatraversal::ActionSetTraversal::ActionSetTraversal(int numberOfActionSets):
  _actionSets(numberOfActionSets) {
}


peano::datatraversal::ActionSetTraversal::~ActionSetTraversal() {
}


int peano::datatraversal::ActionSetTraversal::getMaximumPath() const {
  return static_cast<int>(_actionSets.size());
}


int peano::datatraversal::ActionSetTraversal::getMaximumGrainSize() const {
  int result = 0;
  for (std::vector< ActionSet >::const_iterator p=_actionSets.begin(); p<_actionSets.end(); p++) {
    result = result > p->getNumberOfParallelActions() ? result : p->getNumberOfParallelActions();
  }
  return result;
}


const peano::datatraversal::ActionSet&
peano::datatraversal::ActionSetTraversal::getActionSet( int i ) const {
  assertion( i>=0 );
  assertion( i < getMaximumPath() );
  return _actionSets[i];
}


peano::datatraversal::ActionSet&
peano::datatraversal::ActionSetTraversal::getActionSet( int i ) {
  assertion1( i>=0, toString() );
  assertion3( i < getMaximumPath(), i, getMaximumPath(), toString() );
  return _actionSets[i];
}



#if defined(CacheActionSets)
const peano::datatraversal::ActionSetTraversal&
peano::datatraversal::ActionSetTraversal::getParallelCartesianGridTraversal(const tarch::la::Vector<DIMENSIONS,int>& numberOfCells) {
  static std::vector<ActionSetTraversal*> cachedTraversals;

  if (cachedTraversals.size()==0) {
    cachedTraversals.push_back(0);
  }

  const int key = getKey( numberOfCells );

  if (key==0) {
    if (cachedTraversals[0]!=0) {
      delete cachedTraversals[0];
    }
    cachedTraversals[0] = createParallelCartesianGridTraversal(numberOfCells);
  }
  else {
    for (int i=static_cast<int>(cachedTraversals.size()); i<=key; i++) {
      cachedTraversals.push_back(createParallelCartesianGridTraversal(getNumberOfCellsFromKey(i)));
    }
  }

  // should be an equals, but I haven't redefined == yet
  // this comparison is valid, but it needs way too long too validate usually
//  assertionEquals2( cachedTraversals[key].toString(), createParallelCartesianGridTraversal(numberOfCells).toString(), key, numberOfCells );

  return *cachedTraversals[key];
}
#else
peano::datatraversal::ActionSetTraversal
peano::datatraversal::ActionSetTraversal::getParallelCartesianGridTraversal(const tarch::la::Vector<DIMENSIONS,int>& numberOfCells) {
  return createParallelCartesianGridTraversal(numberOfCells);
}
#endif



#if defined(CacheActionSets)
peano::datatraversal::ActionSetTraversal*
#else
peano::datatraversal::ActionSetTraversal
#endif
peano::datatraversal::ActionSetTraversal::createParallelCartesianGridTraversal(const tarch::la::Vector<DIMENSIONS,int>& numberOfCells) {

  #if defined(CacheActionSets)
  ActionSetTraversal*  returnResult = new ActionSetTraversal(TWO_POWER_D);
  ActionSetTraversal&  result = *returnResult;
  #else
  ActionSetTraversal   returnResult(TWO_POWER_D);
  ActionSetTraversal&  result = returnResult;
  #endif


  tarch::la::Vector<DIMENSIONS,int> maxCellsPerSet = numberOfCells/2 + 1;
  dfor2(i)
    dfor(k,maxCellsPerSet) {
      tarch::la::Vector<DIMENSIONS,int> currentCell = k * 2 + i;
      bool isInsideDomain = true;
      for (int d=0; d<DIMENSIONS; d++) {
        isInsideDomain &= currentCell(d) < numberOfCells(d);
      }
      if (isInsideDomain) {
        result._actionSets[ iScalar ].addAction( Action(currentCell,0) );
      }
    }
  enddforx

  return returnResult;
}


tarch::la::Vector<DIMENSIONS,int>
peano::datatraversal::ActionSetTraversal::getNumberOfCellsFromKey( int key ) {
  assertion( key>=0 );
  // 3->1
  // 6->2
  return tarch::la::Vector<DIMENSIONS,int>(threePowI(key));
}

int peano::datatraversal::ActionSetTraversal::getKey(const tarch::la::Vector<DIMENSIONS,int>& numberOfCells) {
  int cells = numberOfCells(0);
  for (int d=1; d<DIMENSIONS; d++) {
    if (cells!=numberOfCells(d)) cells = 0;
  }

  int currentKey = 0;
  int currentValue = 1;
  while (currentValue<cells) {
    currentValue *= 3;
    currentKey++;
  }

  return currentValue==cells ? currentKey : 0;
}



#if defined(CacheActionSets)
const peano::datatraversal::ActionSetTraversal&
peano::datatraversal::ActionSetTraversal::getSequentialCartesianGridTraversal(const tarch::la::Vector<DIMENSIONS,int>& numberOfCells) {
  static std::vector<ActionSetTraversal*> cachedTraversals;

  if (cachedTraversals.size()==0) {
    cachedTraversals.push_back(0);
  }

  const int key = getKey( numberOfCells );

  if (key==0) {
    if (cachedTraversals[0]!=0) {
      delete cachedTraversals[0];
    }
    cachedTraversals[0] = createSequentialCartesianGridTraversal(numberOfCells);
  }
  else {
    for (int i=static_cast<int>(cachedTraversals.size()); i<=key; i++) {
      cachedTraversals.push_back( createSequentialCartesianGridTraversal(getNumberOfCellsFromKey(i)) );
    }
  }

  // should be an equals, but I haven't redefined == yet
  // this comparison is valid, but it needs way too long too validate usually
//  assertionEquals2( cachedTraversals[key].toString(), createSequentialCartesianGridTraversal(numberOfCells).toString(), key, numberOfCells );

  return *cachedTraversals[key];
}
#else
peano::datatraversal::ActionSetTraversal
peano::datatraversal::ActionSetTraversal::getSequentialCartesianGridTraversal(const tarch::la::Vector<DIMENSIONS,int>& numberOfCells) {
  return createSequentialCartesianGridTraversal(numberOfCells);
}
#endif



#if defined(CacheActionSets)
peano::datatraversal::ActionSetTraversal*
#else
peano::datatraversal::ActionSetTraversal
#endif
peano::datatraversal::ActionSetTraversal::createSequentialCartesianGridTraversal(const tarch::la::Vector<DIMENSIONS,int>& numberOfCells) {
  int maxPath = 1;

  maxPath = 1;
  for (int d=0; d<DIMENSIONS; d++) {
    maxPath *= numberOfCells(d);
  }


  #if defined(CacheActionSets)
  ActionSetTraversal*  returnResult = new ActionSetTraversal(maxPath);
  ActionSetTraversal&  result = *returnResult;
  #else
  ActionSetTraversal   returnResult(maxPath);
  ActionSetTraversal&  result = returnResult;
  #endif


  maxPath = 0;
  dfor(currentCell,numberOfCells) {
    result._actionSets[ maxPath ].addAction( Action(currentCell,0) );
    maxPath++;
  }

  return returnResult;
}


std::string peano::datatraversal::ActionSetTraversal::toString() const {
  std::ostringstream out;
  out << "(";

  for (int i=0; i<getMaximumPath(); i++) {
    out << "{";
    for (int j=0; j<_actionSets[i].getNumberOfParallelActions(); j++) {
      out << "(x=";
      out << _actionSets[i].getAction(j)._cartesianPosition;
      out << ",id=";
      out << _actionSets[i].getAction(j)._id;
      out << ")";
    }
    out << "}";
  }

  out << ")";
  return out.str();
}


peano::datatraversal::ActionSetTraversal
peano::datatraversal::ActionSetTraversal::sortIntoIdBins(const ActionSetTraversal& input) {
  peano::datatraversal::ActionSetTraversal result(0);

  for (int i=0; i<input.getMaximumPath(); i++) {
	assertion2( input.getActionSet(i).getNumberOfParallelActions()==1, i, input.getActionSet(i).getNumberOfParallelActions() );
	int  newId = input.getActionSet(i).getAction(0)._id;
	int  currentActionSet = 0;
	bool holdsIdAlready   = true;
	while (currentActionSet<result.getMaximumPath() && holdsIdAlready) {
      holdsIdAlready = false;
      for (int j=0; j<result.getActionSet(currentActionSet).getNumberOfParallelActions(); j++) {
    	holdsIdAlready |= result.getActionSet(currentActionSet).getAction(j)._id == newId;
      }
      if (holdsIdAlready) {
        currentActionSet++;
      }
	}
	if (currentActionSet==result.getMaximumPath()) {
	  result._actionSets.push_back( ActionSet() );
	}
    result.getActionSet(currentActionSet).addAction(input.getActionSet(i).getAction(0));
  }

  return result;
}
