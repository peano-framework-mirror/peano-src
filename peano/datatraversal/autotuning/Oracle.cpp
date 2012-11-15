#include "peano/datatraversal/autotuning/Oracle.h"
#include "tarch/Assertions.h"
#include "tarch/multicore/MulticoreDefinitions.h"
#include "tarch/multicore/Lock.h"

#if defined(SharedCobra)
#include "tarch/multicore/cobra/Core.h"
#endif


tarch::logging::Log  peano::datatraversal::autotuning::Oracle::_log( "peano::datatraversal::autotuning::Oracle" ) ;


peano::datatraversal::autotuning::Oracle& peano::datatraversal::autotuning::Oracle::getInstance() {
  static peano::datatraversal::autotuning::Oracle singleton;
  return singleton;
}


peano::datatraversal::autotuning::Oracle::Oracle():
  _watchSinceLastSwitchCall("peano::datatraversal::autotuning::Oracle", "Oracle()", true),
  _oracles(),
  _watches(),
  _currentOracle(0),
  _booleanSemaphore(),
  _oraclePrototype(0) {
}


void peano::datatraversal::autotuning::Oracle::setOracle( OracleForOnePhase* oraclePrototype ) {
  tarch::multicore::Lock scopeLock( _booleanSemaphore );

  assertion( oraclePrototype!=0 );

  if (_oraclePrototype!=0) {
    delete _oraclePrototype;
  }
  _oraclePrototype = oraclePrototype;

  int numberOfOracles = static_cast<int>( _oracles.size() );

  deleteOracles();
  createOracles(numberOfOracles);
}


void peano::datatraversal::autotuning::Oracle::plotStatistics() {
  logInfo( "plotStatistics()", "used " << _watches.size() << " oracle(s) ");

  for (
    OracleContainer::const_iterator p = _oracles.begin();
    p != _oracles.end();
    p++
  ) {
    if ( p->first._phase >= AdapterStatesReservedForRepostorySteering ) {
      p->second->plotStatistics();
    }
  }
}


peano::datatraversal::autotuning::Oracle::~Oracle() {
  deleteOracles();
  delete _oraclePrototype;
  _oraclePrototype = 0;
}


void peano::datatraversal::autotuning::Oracle::setNumberOfOracles(int value) {
  assertion( value>0 );

  tarch::multicore::Lock scopeLock( _booleanSemaphore );
  deleteOracles();
  createOracles(value);
}


void peano::datatraversal::autotuning::Oracle::createOracles(int numberOfOracles) {
  #if defined(SharedMemoryParallelisation)

  if (_oraclePrototype==0) {
	  logWarning( "createOracles(int)", "no oracle type configured. Perhaps forgot to call peano::datatraversal::autotuning::Oracle::setOracle()" );
  }
  else {
    assertion( _oracles.size()==0 );
    assertion( _watches.size()==0 );
	  assertion( _oraclePrototype!=0 );
	  for (int i=0; i<numberOfOracles; i++) {
	    for (int j=0; j<NumberOfDifferentMethodsCalling; j++) {
	      OracleKey currentKey;
	      currentKey._phase        = i;
	      currentKey._askingMethod = toMethodTrace(j);

        _oracles[currentKey]                               = _oraclePrototype->createNewOracle(currentKey._phase - AdapterStatesReservedForRepostorySteering + 1,currentKey._askingMethod);
        _watches[currentKey]._recursiveCallsForThisOracle  = 0;
        _watches[currentKey]._measureTime                  = false;
        _watches[currentKey]._watch                        = new tarch::timing::Watch("peano::datatraversal::autotuning::Oracle", "createOracles(int)", false);
	    }
    }
  }
  #endif
}


void peano::datatraversal::autotuning::Oracle::deleteOracles() {
  const int NumberOfOracles = static_cast<int>(_oracles.size()) / NumberOfDifferentMethodsCalling;
  for (int i=0; i<NumberOfOracles; i++) {
    for (int j=0; j<NumberOfDifferentMethodsCalling; j++) {
      OracleKey currentKey;
      currentKey._phase        = i;
      currentKey._askingMethod = toMethodTrace(j);

      delete _oracles[currentKey];
      delete _watches[currentKey]._watch;

      _oracles.erase(currentKey);
      _watches.erase(currentKey);
    }
  }

  assertion( _oracles.empty() );
  assertion( _watches.empty() );
}


void peano::datatraversal::autotuning::Oracle::switchToOracle(int id) {
  #if defined(SharedMemoryParallelisation)
  assertion1( id>=0, id );
  assertion3( id<static_cast<int>(_oracles.size()), id, _oracles.size(), "see method's comment" );

  #if defined(SharedCobra)
  tarch::multicore::cobra::Core::getInstance().getScheduler().gc();
  #endif

  tarch::multicore::Lock scopeLock( _booleanSemaphore );

  _watchSinceLastSwitchCall.stopTimer();

  assertion(_oraclePrototype!=0);
  _oraclePrototype->informAboutElapsedTimeOfLastTraversal(_watchSinceLastSwitchCall.getCalendarTime());

  _currentOracle=id;

  _watchSinceLastSwitchCall.startTimer();
  #endif
}


int peano::datatraversal::autotuning::Oracle::parallelise(int problemSize, MethodTrace askingMethod ) {
  logTraceInWith2Arguments( "parallelise(int,int)", problemSize, askingMethod );

  int result;

  #if defined(SharedMemoryParallelisation)
  if (_oraclePrototype==0) {
    logWarning( "parallelise(int,Trace)", "no oracle type configured. Perhaps forgot to call peano::datatraversal::autotuning::Oracle::setOracle()" );
    result = 0;
  }
  else {
    assertion( _currentOracle>=0 );
    assertion2( _currentOracle<static_cast<int>(_oracles.size()),  _currentOracle, _oracles.size() );

    tarch::multicore::Lock scopeLock( _booleanSemaphore );

    _lastKeyUsed._phase        = _currentOracle;
    _lastKeyUsed._askingMethod = askingMethod;

    assertion3(_watches.count(_lastKeyUsed)>0, _currentOracle, toString(askingMethod), askingMethod );
    assertion3(_oracles.count(_lastKeyUsed)>0, _currentOracle, toString(askingMethod), askingMethod );

    _watches[_lastKeyUsed]._recursiveCallsForThisOracle++;

    if (_watches[_lastKeyUsed]._recursiveCallsForThisOracle==1) {
      std::pair<int,bool> oracleDescision =  _oracles[_lastKeyUsed]->parallelise(problemSize);;
      result = oracleDescision.first;
      _watches[_lastKeyUsed]._measureTime = oracleDescision.second;
      if ( oracleDescision.second ) {
        _watches[_lastKeyUsed]._watch->startTimer();
      }
    }
    else {
      result = 0;
    }
  }
  #else
  result = 0;
  #endif

  assertion3( result >=0, result, problemSize, toString(askingMethod) );
  assertion3( result <= problemSize, result, problemSize, toString(askingMethod) );

  logTraceOutWith1Argument( "parallelise(int,int)", result );
  return result;
}


void peano::datatraversal::autotuning::Oracle::parallelSectionHasTerminated(MethodTrace askingMethod) {
  logTraceInWith1Argument( "parallelSectionHasTerminated(...)", toString(askingMethod) );
  #if defined(SharedMemoryParallelisation)
  if (_oraclePrototype!=0) {
    assertion( _currentOracle>=0 );
    assertion2( _currentOracle<static_cast<int>(_oracles.size()),  _currentOracle, _oracles.size() );

    tarch::multicore::Lock scopeLock( _booleanSemaphore );

    OracleKey currentKey;
    currentKey._phase        = _currentOracle;
    currentKey._askingMethod = askingMethod;

    assertion(_watches.count(currentKey)>0);
    assertion(_oracles.count(currentKey)>0);

    assertion3( _watches[currentKey]._recursiveCallsForThisOracle>0, toString(askingMethod), currentKey._phase, toString(currentKey._askingMethod) );

    _watches[currentKey]._recursiveCallsForThisOracle--;

    if (_watches[currentKey]._recursiveCallsForThisOracle==0 && _watches[currentKey]._measureTime ) {
      _watches[currentKey]._watch->stopTimer();
      _oracles[currentKey]->parallelSectionHasTerminated(_watches[currentKey]._watch->getCalendarTime());
    }
  }
  #endif
  logTraceOut( "parallelSectionHasTerminated(...)" );
}


peano::datatraversal::autotuning::Oracle::OracleKey::OracleKey(int phase, MethodTrace askingMethod):
  _phase(phase),
  _askingMethod(askingMethod) {
  assertion( phase>0 );
  assertion( askingMethod>=0 );
  assertion( askingMethod<NumberOfDifferentMethodsCalling );
}


peano::datatraversal::autotuning::Oracle::OracleKey::OracleKey() {
}


peano::datatraversal::autotuning::Oracle::OracleKey::~OracleKey() {
}


bool peano::datatraversal::autotuning::Oracle::OracleKey::operator<(const peano::datatraversal::autotuning::Oracle::OracleKey& rhs) const {
  if (_askingMethod < rhs._askingMethod) return true;
  if (_askingMethod > rhs._askingMethod) return false;

  return _phase < rhs._phase;
}


bool peano::datatraversal::autotuning::Oracle::OracleKey::operator==(const peano::datatraversal::autotuning::Oracle::OracleKey& rhs) const {
  return (_phase == rhs._phase) && (_askingMethod == rhs._askingMethod);
}
