#include "peano/datatraversal/autotuning/OracleForOnePhaseUsingAllThreads.h"
#include "tarch/Assertions.h"

#include <cstdlib>


tarch::logging::Log  peano::datatraversal::autotuning::OracleForOnePhaseUsingAllThreads::_log( "peano::datatraversal::autotuning::OracleForOnePhaseUsingAllThreads" );


peano::datatraversal::autotuning::OracleForOnePhaseUsingAllThreads::OracleForOnePhaseUsingAllThreads(
  int numberOfThreads,
  int splitTheTree,
  bool pipelineDescendProcessing,
  bool pipelineAscendProcessing,
  const MethodTrace& methodTrace
):
  _numberOfThreads(numberOfThreads),
  _executionTime(1.0),
  _methodTrace(methodTrace),
  _splitTheTree(splitTheTree),
  _pipelineDescendProcessing(pipelineDescendProcessing),
  _pipelineAscendProcessing(pipelineAscendProcessing) {
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
    ) &&
    _splitTheTree !=2
  )
    {
    const int divisor   = 2 << _numberOfThreads;
    const int grainSize = problemSize / divisor;
    return std::pair<int,bool>(grainSize<1 ? 1 : grainSize,true);
  }
  else if (
    _splitTheTree > 0  &&
    (
      _methodTrace == SplitLoadVerticesTaskOnRegularStationaryGrid
      ||
      _methodTrace == SplitStoreVerticesTaskOnRegularStationaryGrid
    )
  ) {
    const int grainSize = problemSize / _numberOfThreads;
    return std::pair<int,bool>(grainSize<1 ? 1 : grainSize,true);
  }
  else if ( _pipelineAscendProcessing && _methodTrace == PipelineAscendTask ) {
    return std::pair<int,bool>(1,true);
  }
  else if ( _pipelineDescendProcessing && _methodTrace == PipelineDescendTask ) {
    return std::pair<int,bool>(1,true);
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
  return new OracleForOnePhaseUsingAllThreads(_numberOfThreads,_splitTheTree,_pipelineDescendProcessing, _pipelineAscendProcessing,methodTrace);
}


void peano::datatraversal::autotuning::OracleForOnePhaseUsingAllThreads::informAboutElapsedTimeOfLastTraversal(double elapsedTime) {
}
