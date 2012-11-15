#include "peano/datatraversal/autotuning/OracleForOnePhaseWithFixedGrainSize.h"
#include "tarch/Assertions.h"

#include <cstdlib>


tarch::logging::Log  peano::datatraversal::autotuning::OracleForOnePhaseWithFixedGrainSize::_log( "peano::datatraversal::autotuning::OracleForOnePhaseWithFixedGrainSize" );


peano::datatraversal::autotuning::OracleForOnePhaseWithFixedGrainSize::OracleForOnePhaseWithFixedGrainSize(int grainSize):
  _fixedGrainSize(grainSize),
  _executionTime(1.0) {
  assertion( _fixedGrainSize>=0 );
}


std::pair<int,bool> peano::datatraversal::autotuning::OracleForOnePhaseWithFixedGrainSize::parallelise(int problemSize) {
  return std::pair<int,bool>(problemSize<=_fixedGrainSize ? problemSize-1 : _fixedGrainSize,true);
}


void peano::datatraversal::autotuning::OracleForOnePhaseWithFixedGrainSize::parallelSectionHasTerminated(double elapsedCalendarTime) {
  _executionTime.setValue( elapsedCalendarTime );
}


void peano::datatraversal::autotuning::OracleForOnePhaseWithFixedGrainSize::plotStatistics() const {
  logInfo(
    "plotRuntimes()",
    "averaged runtime with grain size " << _fixedGrainSize << ": " <<
    _executionTime.toString()
  );
}


peano::datatraversal::autotuning::OracleForOnePhaseWithFixedGrainSize::~OracleForOnePhaseWithFixedGrainSize() {
}


peano::datatraversal::autotuning::OracleForOnePhase* peano::datatraversal::autotuning::OracleForOnePhaseWithFixedGrainSize::createNewOracle(int adapterNumber, const MethodTrace& methodTrace) const {
  return new OracleForOnePhaseWithFixedGrainSize(_fixedGrainSize);
}


void peano::datatraversal::autotuning::OracleForOnePhaseWithFixedGrainSize::informAboutElapsedTimeOfLastTraversal(double elapsedTime) {
}
