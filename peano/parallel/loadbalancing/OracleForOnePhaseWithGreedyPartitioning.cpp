#include "peano/parallel/loadbalancing/OracleForOnePhaseWithGreedyPartitioning.h"

#include "tarch/Assertions.h"
#include "tarch/la/Scalar.h"


tarch::logging::Log peano::parallel::loadbalancing::OracleForOnePhaseWithGreedyPartitioning::_log( "peano::parallel::loadbalancing::OracleForOnePhaseWithGreedyPartitioning" );


bool peano::parallel::loadbalancing::OracleForOnePhaseWithGreedyPartitioning::_forkHasFailed = false;



peano::parallel::loadbalancing::OracleForOnePhaseWithGreedyPartitioning::OracleForOnePhaseWithGreedyPartitioning(bool joinsAllowed):
  _joinsAllowed(joinsAllowed),
  _idleWorkers() {
}


peano::parallel::loadbalancing::OracleForOnePhaseWithGreedyPartitioning::~OracleForOnePhaseWithGreedyPartitioning() {
}


void peano::parallel::loadbalancing::OracleForOnePhaseWithGreedyPartitioning::receivedStartCommand(const LoadBalancingFlag& commandFromMaster ) {
  if (commandFromMaster==Join) {
    _idleWorkers.clear();
  }
}


peano::parallel::loadbalancing::LoadBalancingFlag peano::parallel::loadbalancing::OracleForOnePhaseWithGreedyPartitioning::getCommandForWorker( int workerRank, bool forkIsAllowed, bool joinIsAllowed ) {
  logTraceInWith5Arguments( "getCommandForWorker(int,bool)", workerRank, forkIsAllowed, joinIsAllowed, _joinsAllowed, _idleWorkers.count(workerRank) );

  LoadBalancingFlag result = Continue;

  if ( joinIsAllowed
    && _joinsAllowed
    && _idleWorkers.count(workerRank)>0
  ) {
    _idleWorkers.clear();
    result = Join;
  }
  else if (!_forkHasFailed && forkIsAllowed) {
    result = ForkGreedy;
  }

  logTraceOutWith1Argument( "getCommandForWorker(int,bool)", toString(result) );
  return result;
}


void peano::parallel::loadbalancing::OracleForOnePhaseWithGreedyPartitioning::receivedTerminateCommand( int workerRank, double waitedTime, double workerCells) {
  if ( tarch::la::equals( workerCells, tarch::la::NUMERICAL_ZERO_DIFFERENCE ) ) {
    _idleWorkers.insert( workerRank );
  }
}


void peano::parallel::loadbalancing::OracleForOnePhaseWithGreedyPartitioning::plotStatistics() {
}


peano::parallel::loadbalancing::OracleForOnePhaseWithGreedyPartitioning::OracleForOnePhase* peano::parallel::loadbalancing::OracleForOnePhaseWithGreedyPartitioning::createNewOracle(int adapterNumber) const {
  return new OracleForOnePhaseWithGreedyPartitioning(_joinsAllowed);
}


void peano::parallel::loadbalancing::OracleForOnePhaseWithGreedyPartitioning::forkFailed() {
  _forkHasFailed = true;
}