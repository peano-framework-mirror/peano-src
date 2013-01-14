// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_DATA_TRAVERSAL_AUTOTUNING_ORACLE_FOR_ONE_PHASE_USING_ALL_THREADS_H_
#define _PEANO_DATA_TRAVERSAL_AUTOTUNING_ORACLE_FOR_ONE_PHASE_USING_ALL_THREADS_H_


#include "tarch/logging/Log.h"
#include "peano/datatraversal/autotuning/OracleForOnePhase.h"
#include "tarch/timing/Measurement.h"


#include <map>


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
    static tarch::logging::Log                 _log;

    int                                        _numberOfThreads;
    std::map<int, tarch::timing::Measurement>  _executionTime;
    const MethodTrace                          _methodTrace;
    const int                                  _splitTheTree;
    const bool                                 _pipelineDescendProcessing;
    const bool                                 _pipelineAscendProcessing;

    int                                        _lastProblemSize;
  public:
    /**
     * Use a fixed number of threads.
     *
     * @param splitTheTree (0=no, 1=yes and parallelise, 2=yes, but do not parallelise any events on the regular subgrid
     */
    OracleForOnePhaseUsingAllThreads(int numberOfThreads, int splitTheTree = 1, bool pipelineDescendProcessing = false, bool pipelineAscendProcessing = false, const MethodTrace& methodTrace = NumberOfDifferentMethodsCalling);

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
