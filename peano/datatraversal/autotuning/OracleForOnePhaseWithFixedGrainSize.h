// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_DATA_TRAVERSAL_AUTOTUNING_ORACLE_FOR_ONE_PHASE_WITH_FIXED_GRAIN_SIZE_H_
#define _PEANO_DATA_TRAVERSAL_AUTOTUNING_ORACLE_FOR_ONE_PHASE_WITH_FIXED_GRAIN_SIZE_H_


#include "tarch/logging/Log.h"
#include "peano/datatraversal/autotuning/OracleForOnePhase.h"
#include "tarch/timing/Measurement.h"


#include <map>
#include <vector>


namespace peano {
  namespace datatraversal {
    namespace autotuning {
      class OracleForOnePhaseWithFixedGrainSize;
    }
  }
}


/**
 * @author Tobias Weinzierl
 */
class peano::datatraversal::autotuning::OracleForOnePhaseWithFixedGrainSize: public peano::datatraversal::autotuning::OracleForOnePhase {
  private:
    static tarch::logging::Log  _log;

    int                         _fixedGrainSize;
    tarch::timing::Measurement  _executionTime;
  public:
    OracleForOnePhaseWithFixedGrainSize(int grainSize);

    virtual ~OracleForOnePhaseWithFixedGrainSize();

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
