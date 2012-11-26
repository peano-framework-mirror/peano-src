// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_DATA_TRAVERSAL_AUTOTUNING_ORACLE_FOR_ONE_PHASE_USING_ALL_THREADS_H_
#define _PEANO_DATA_TRAVERSAL_AUTOTUNING_ORACLE_FOR_ONE_PHASE_USING_ALL_THREADS_H_


#include "tarch/logging/Log.h"
#include "peano/datatraversal/autotuning/OracleForOnePhase.h"
#include "tarch/timing/Measurement.h"


#include <map>
#include <vector>


namespace peano {
  namespace datatraversal {
    namespace autotuning {
      class OracleForOnePhaseUsingAllThreads;
    }
  }
}


/**
 * @author Tobias Weinzierl
 */
class peano::datatraversal::autotuning::OracleForOnePhaseUsingAllThreads: public peano::datatraversal::autotuning::OracleForOnePhase {
  private:
    static tarch::logging::Log  _log;

    int                         _numberOfThreads;
    tarch::timing::Measurement  _executionTime;
    const MethodTrace           _methodTrace;
  public:
    /**
     * Use a fixed number of threads.
     */
    OracleForOnePhaseUsingAllThreads(int numberOfThreads, const MethodTrace& methodTrace = NumberOfDifferentMethodsCalling);

    virtual ~OracleForOnePhaseUsingAllThreads();

    virtual std::pair<int,bool> parallelise(int problemSize);
    virtual void parallelSectionHasTerminated(double elapsedCalendarTime);
    virtual void plotStatistics() const;

    virtual void informAboutElapsedTimeOfLastTraversal(double elapsedTime);

    /**
     * For this oracle type, the adapter number is completely irrelevant.
     */
    virtual OracleForOnePhase* createNewOracle(int adapterNumber, const MethodTrace& methodTrace) const;
};


#endif
