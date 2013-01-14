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
  _executionTime(),
  _methodTrace(methodTrace),
  _splitTheTree(splitTheTree),
  _pipelineDescendProcessing(pipelineDescendProcessing),
  _pipelineAscendProcessing(pipelineAscendProcessing),
  _lastProblemSize(-1) {
  assertion( _numberOfThreads>=1 );
}


std::pair<int,bool> peano::datatraversal::autotuning::OracleForOnePhaseUsingAllThreads::parallelise(int problemSize) {
  assertionEquals1( _lastProblemSize, -1, toString(_methodTrace) );
  _lastProblemSize = problemSize;
  if (_numberOfThreads==1) {
    return std::pair<int,bool>(0,true);
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
    _lastProblemSize = -1;
    return std::pair<int,bool>(0,false);
  }
}


void peano::datatraversal::autotuning::OracleForOnePhaseUsingAllThreads::parallelSectionHasTerminated(double elapsedCalendarTime) {
  assertion( _lastProblemSize>0 );
  _executionTime[_lastProblemSize].setValue( elapsedCalendarTime );
  _executionTime[_lastProblemSize].setAccuracy(1.0e-5);
  _lastProblemSize = -1;
}


void peano::datatraversal::autotuning::OracleForOnePhaseUsingAllThreads::plotStatistics() const {
  for (std::map<int, tarch::timing::Measurement>::const_iterator p=_executionTime.begin(); p!=_executionTime.end(); p++) {
    if (p->second.getNumberOfMeasurements()>0) {
      logInfo(
        "plotRuntimes()",
        "averaged runtime for " << _numberOfThreads << " thread(s) in phase " << toString(_methodTrace)
        << " for problem size " << p->first << ": " <<
        p->second.toString()
      );
    }
  }
}


peano::datatraversal::autotuning::OracleForOnePhaseUsingAllThreads::~OracleForOnePhaseUsingAllThreads() {
}


peano::datatraversal::autotuning::OracleForOnePhase* peano::datatraversal::autotuning::OracleForOnePhaseUsingAllThreads::createNewOracle(int adapterNumber, const MethodTrace& methodTrace) const {
  return new OracleForOnePhaseUsingAllThreads(_numberOfThreads,_splitTheTree,_pipelineDescendProcessing, _pipelineAscendProcessing,methodTrace);
}


void peano::datatraversal::autotuning::OracleForOnePhaseUsingAllThreads::informAboutElapsedTimeOfLastTraversal(double elapsedTime) {
}
