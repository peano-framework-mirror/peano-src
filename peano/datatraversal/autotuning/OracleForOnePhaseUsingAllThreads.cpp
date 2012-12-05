#include "peano/datatraversal/autotuning/OracleForOnePhaseUsingAllThreads.h"
#include "tarch/Assertions.h"

#include <cstdlib>


tarch::logging::Log  peano::datatraversal::autotuning::OracleForOnePhaseUsingAllThreads::_log( "peano::datatraversal::autotuning::OracleForOnePhaseUsingAllThreads" );


peano::datatraversal::autotuning::OracleForOnePhaseUsingAllThreads::OracleForOnePhaseUsingAllThreads(int numberOfThreads, const MethodTrace& methodTrace):
  _numberOfThreads(numberOfThreads),
  _executionTime(1.0),
  _methodTrace(methodTrace) {
  assertion( _numberOfThreads>=1 );
}


std::pair<int,bool> peano::datatraversal::autotuning::OracleForOnePhaseUsingAllThreads::parallelise(int problemSize) {
  if (_numberOfThreads==1) {
    return std::pair<int,bool>(0,false);
  }
  else if (
    (
      _methodTrace==CallEnterCellOnRegularStationaryGrid
      ||
      _methodTrace==CallLeaveCellOnRegularStationaryGrid
      ||
      _methodTrace==CallTouchFirstTimeOnRegularStationaryGrid
      ||
      _methodTrace==CallTouchLastTimeOnRegularStationaryGrid
      ||
      _methodTrace==AscendOnRegularStationaryGrid
      ||
      _methodTrace==DescendOnRegularStationaryGrid
      ||
      _methodTrace==SplitLoadVerticesTaskOnRegularStationaryGrid
      ||
      _methodTrace==SplitStoreVerticesTaskOnRegularStationaryGrid
    )
  )
    {
    return std::pair<int,bool>(problemSize<_numberOfThreads ? 1 : problemSize/_numberOfThreads,true);
  }
  else {
    return std::pair<int,bool>(0,false);
  }
}


void peano::datatraversal::autotuning::OracleForOnePhaseUsingAllThreads::parallelSectionHasTerminated(double elapsedCalendarTime) {
  _executionTime.setValue( elapsedCalendarTime );
}


void peano::datatraversal::autotuning::OracleForOnePhaseUsingAllThreads::plotStatistics() const {
  logInfo(
    "plotRuntimes()",
    "averaged runtime for " << _numberOfThreads << " thread(s) in phase " << toString(_methodTrace) << ": " <<
    _executionTime.toString()
  );
}


peano::datatraversal::autotuning::OracleForOnePhaseUsingAllThreads::~OracleForOnePhaseUsingAllThreads() {
}


peano::datatraversal::autotuning::OracleForOnePhase* peano::datatraversal::autotuning::OracleForOnePhaseUsingAllThreads::createNewOracle(int adapterNumber, const MethodTrace& methodTrace) const {
  return new OracleForOnePhaseUsingAllThreads(_numberOfThreads,methodTrace);
}


void peano::datatraversal::autotuning::OracleForOnePhaseUsingAllThreads::informAboutElapsedTimeOfLastTraversal(double elapsedTime) {
}
