// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_PARALLEL_LOAD_BALANCING_ORACLE_H_
#define _PEANO_PARALLEL_LOAD_BALANCING_ORACLE_H_


#include "tarch/timing/Watch.h"
#include "tarch/la/Vector.h"
#include "peano/parallel/loadbalancing/OracleForOnePhase.h"
#include "peano/utils/Globals.h"


#include <vector>


namespace peano {
  namespace parallel {
    namespace loadbalancing {
      class Oracle;
    }
  }
}


/**
 * Load balancing oracle
 *
 * The load distribution in Peano is (logically) a tree topology. Whenever the
 * grid is traversed, basically startup messages are passed top-down and state
 * messages are passed bottom-up. Such a scheme naturally yields master-worker
 * relations. Whenever data or algorithms are duplicated, always the master has
 * the responsibility and the correct data. It is never the worker.
 *
 * The oracles run on every node. Whenever the tree traversal encounters yet
 * another worker, it asks the local oracle what to do with this worker, i.e.
 * the master decides on the worker. In turn, the oracle on the worker always
 * is informed about the master's decision. It may not veto, but it is
 * informed.
 *
 * @author Tobias Weinzierl
 */
class peano::parallel::loadbalancing::Oracle {
  private:
    struct Worker {
      int                                   _rank;
      tarch::la::Vector<DIMENSIONS,double>  _boundingBoxOffset;
      tarch::la::Vector<DIMENSIONS,double>  _boundingBoxSize;
      bool                                  _hasWorker;

      Worker(int rank, const tarch::la::Vector<DIMENSIONS,double>& boundingBoxOffset, const tarch::la::Vector<DIMENSIONS,double>& boundingBoxSize);
    };

    static tarch::logging::Log  _log;

    /**
     * Set of oracles.
     */
    typedef std::vector<OracleForOnePhase*>  OracleDatabase;

    /**
     * Maps ranks to oracles
     */
    OracleDatabase                           _oracles;

    /**
     * Current active oracle.
     *
     * Identifies which oracle out of _oracles to chose.
     */
    int                                      _currentOracle;

    /**
     * Maps each oracle type to a two tuple with a watch and the number of
     * invocations for this watch.
     */
    tarch::timing::Watch                     _watch;

    OracleForOnePhase*                       _oraclePrototype;

    typedef std::vector<Worker>              WorkerContainer;

    /**
     * It is basically a set. However, I wanna access it both element-wisely and in a
     * stack manner.
     */
    WorkerContainer                          _workers;

    LoadBalancingFlag                        _startCommand;

    void createOracles(int numberOfOracles);
    void deleteOracles();

    Oracle();

    WorkerContainer::iterator       getWorkerRecord( int rank );
    WorkerContainer::const_iterator getWorkerRecord( int rank ) const;
  public:
    virtual ~Oracle();

    static Oracle& getInstance();

    /**
     * Inform the oracle about a new worker. This worker may not be contained
     * in the database before. The worker is appended, i.e. from now on, it is
     * the last one.
     *
     * @param boundingBoxOffset  Information about the worker's domain
     * @param boundingBoxSize    Information about the worker's domain
     */
    void addWorker(
      int                                          rank,
      const tarch::la::Vector<DIMENSIONS,double>&  boundingBoxOffset,
      const tarch::la::Vector<DIMENSIONS,double>&  boundingBoxSize,
      int                                          level
    );

    /**
     * Is public to enable other classes to validate their state as well.
     */
    bool workersListContainsRank( int rank ) const;

    /**
     * Remove a worker from the database. It has to be inserted there before.
     * This operation is not used by the regular grid, as the regular grid
     * always only removes the last worker inserted. For the spacetree grid
     * however it is a different story.
     */
    void removeWorker(int rank);

    /**
     * Removes the last worker added and returns its rank.
     */
    void removeLastWorkerAdded();

    int getRankOfLastWorkerAdded() const;

    bool hasWorkers() const;

    int getNumberOfWorkers() const;

    int getWorkerRank(int index) const;

    tarch::la::Vector<DIMENSIONS,double>  getBoundingBoxOffset( int workerRank ) const;

    tarch::la::Vector<DIMENSIONS,double>  getBoundingBoxSizeOfWorker( int workerRank ) const;

    /**
     * Inform state that there has been a fork call for one worker. It is
     * important for the state to know whether one of its workers has forked
     * again. If this is the case, joins are not allowed.
     *
     * By default, this information is passed bottom-up. See the merge
     * operation. However, if the restriction is switched off, we have no
     * real information. The state thus assumes that the worker really has
     * done the fork and that the fork was successful, i.e. that there have
     * been nodes available. If might turn out to be wrong in the next
     * restriction, but till then, this is all we know.
     */
    void setWorkerHasWorker(int workerRank, bool value);

    bool hasWorkerAWorker(int workerRank) const;

    void plotStatistics();

    /**
     * Tell the oracle how many different adapters you'll gonna use.
     */
    void setNumberOfOracles(int value);

    /**
     * Tell the oracle which adapter is used right now.
     */
    void switchToOracle(int id);

    /**
     * Set the active oracle. Do not delete this argument - the oracle will
     * do it.
     */
    void setOracle( OracleForOnePhase* oraclePrototype );

    void receivedStartCommand(const LoadBalancingFlag& commandFromMaster );

    /**
     * Return last start command
     *
     * Returns the last start command received due to receivedStartCommand().
     * However, also a failed fork might modify the result of this operation.
     *
     * @see forkFailed()
     */
    LoadBalancingFlag getLastStartCommand() const;

    /**
     * Get the command for a worker
     *
     * The operation basically forwards this call to
     * OracleForOnePhase::getCommandForWorker().
     *
     * This operation is not const, as it might update some local stuff.
     *
     * @param workerRank Rank of the worker for which the load balancing would
     *                   be interested in a command
     * @param isRegularGrid For a regular grid, we may only join a partition if
     *                   this was the last partition added.
     */
    LoadBalancingFlag getCommandForWorker( int workerRank, bool isRegularGrid, bool forkIsAllowed = true, bool joinIsAllowed = true);

    /**
     * Notification mechanism that the master is finished. This might not be
     * sent after a getCommandForWorker() call if the bottom-up communication
     * is switched off.
     */
    void receivedTerminateCommand( int workerRank, double workerCells);

    /**
     * Notifies the oracle that from now on, the master starts to wait for workers.
     */
    void masterStartsToWaitForWorkers();

    /**
     * Fork Has Failed
     *
     * Informs the active oracle that a fork has failed. This is usually due to
     * a lack of idle MPI ranks.
     *
     * If the current start command for this oracle is fork, we reset it to
     * continue, as it does make sense to try to fork over and over again if
     * this just doesn't work out.
     */
    void forkFailed();
};



#endif
