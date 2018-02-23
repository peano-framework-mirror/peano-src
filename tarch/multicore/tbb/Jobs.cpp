#include "../Jobs.h"
#include "tarch/multicore/Core.h"

#if defined(SharedTBB)

#include "tarch/logging/Log.h"
#include "tarch/Assertions.h"
#include "tarch/multicore/tbb/Jobs.h"


#include <vector>
#include <limits>


tarch::logging::Log tarch::multicore::jobs::internal::_log( "tarch::multicore::jobs::internal" );


tbb::atomic<int>                                               tarch::multicore::jobs::internal::_numberOfRunningBackgroundJobConsumerTasks(0);
tbb::concurrent_queue<tarch::multicore::jobs::BackgroundJob*>  tarch::multicore::jobs::internal::_backgroundJobs;
tarch::multicore::jobs::internal::JobMap                       tarch::multicore::jobs::internal::_pendingJobs;

//
// This is a bug in Intel's TBB as documented on
//
// https://software.intel.com/en-us/forums/intel-threading-building-blocks/topic/700057
//
// These task groups have to be static inside a cpp file.
//
//tbb::task_group_context                                        tarch::multicore::jobs::internal::BackgroundJobConsumerTask::backgroundTaskContext;
static tbb::task_group_context  backgroundTaskContext(tbb::task_group_context::isolated);
//static tbb::task_group  backgroundTaskContext;
static tbb::task_group_context  importantTaskContext(tbb::task_group_context::isolated);


tarch::multicore::jobs::internal::BackgroundJobConsumerTask::BackgroundJobConsumerTask(int maxJobs):
  _maxJobs(maxJobs) {
}


tarch::multicore::jobs::internal::BackgroundJobConsumerTask::BackgroundJobConsumerTask(const BackgroundJobConsumerTask& copy):
  _maxJobs(copy._maxJobs) {
}


void tarch::multicore::jobs::internal::BackgroundJobConsumerTask::enqueue() {
  _numberOfRunningBackgroundJobConsumerTasks.fetch_and_add(1);
  BackgroundJobConsumerTask* tbbTask = new (tbb::task::allocate_root(backgroundTaskContext)) BackgroundJobConsumerTask(
    std::max( 1, static_cast<int>(_backgroundJobs.unsafe_size())/tarch::multicore::Core::getInstance().getNumberOfThreads() )
  );
  tbb::task::enqueue(*tbbTask);
  backgroundTaskContext.set_priority(tbb::priority_low);
  logDebug( "enqueue()", "spawned new background consumer task" );
}


tbb::task* tarch::multicore::jobs::internal::BackgroundJobConsumerTask::execute() {
  processNumberOfBackgroundJobs(_maxJobs);
  _numberOfRunningBackgroundJobConsumerTasks.fetch_and_add(-1);
  if (!_backgroundJobs.empty()) {
    enqueue();
  }
  return nullptr;
}


tarch::multicore::jobs::internal::JobQueue& tarch::multicore::jobs::internal::getJobQueue( int jobClass ) {
	if ( _pendingJobs.count(jobClass)==0 ) {
    JobMap::accessor    a;
    _pendingJobs.insert( a, jobClass );
	}
  JobMap::accessor c;
  _pendingJobs.find( c, jobClass );
  return c->second;
}


void tarch::multicore::jobs::internal::spawnBlockingJob(
  std::function<void()>&  job,
  tbb::atomic<int>&       semaphore,
  bool                    isTask,
  int                     jobClass
) {
  if ( isTask ) {
    job();
    semaphore.fetch_and_add(-1);
  }
  else {
    getJobQueue(jobClass).jobs.push(
      new JobWithoutCopyOfFunctorAndSemaphore(job, semaphore, isTask, jobClass )
    );

    logDebug( "spawnBlockingJob(...)", "enqueued job. tasks in this queue of class " << jobClass << "=" << getJobQueue(jobClass).jobs.unsafe_size() );
  }
}


bool tarch::multicore::jobs::internal::processNumberOfBackgroundJobs(int maxJobs) {
  logDebug( "processNumberOfBackgroundJobs()", "background consumer task becomes awake" );

  tarch::multicore::jobs::BackgroundJob* myTask = nullptr;
  bool gotOne = _backgroundJobs.try_pop(myTask);
  bool result = false;
  while (gotOne && maxJobs>0) {
    logDebug( "processNumberOfBackgroundJobs()", "consumer task found job to do" );
    const bool reschedule = myTask->run();
    const bool taskHasBeenLongRunning = myTask->isLongRunning();
    if (reschedule) {
      _backgroundJobs.push( myTask );
    }
    else {
      delete myTask;
    }
    maxJobs--;
    result = true;
    if ( maxJobs>0 && !taskHasBeenLongRunning ) {
      gotOne = _backgroundJobs.try_pop(myTask);
    }
    else {
  	gotOne = false;
    }
  }

  logDebug( "processNumberOfBackgroundJobs()", "background task consumer is done and kills itself" );

  return result;
}


void tarch::multicore::jobs::terminateAllPendingBackgroundConsumerJobs() {
  backgroundTaskContext.cancel_group_execution();
}


void tarch::multicore::jobs::spawnBackgroundJob(BackgroundJob* job) {
  BackgroundJobType mode = job->getJobType();

  switch (mode) {
    case BackgroundJobType::ProcessImmediately:
      job->run();
      delete job;
      break;
    case BackgroundJobType::IsTaskAndRunAsSoonAsPossible:
      {
        internal::TBBBackgroundJobWrapper* tbbTask = new(tbb::task::allocate_root(importantTaskContext)) internal::TBBBackgroundJobWrapper(job);
        // we may not use spawn as spawn relies on the current context and thus kills us if this context is already freed
        //        tbb::task::spawn(*tbbTask);
        tbb::task::enqueue(*tbbTask);
        importantTaskContext.set_priority(tbb::priority_high);
      }
      break;
    case BackgroundJobType::BackgroundJob:
      {
        internal::_backgroundJobs.push(job);
        
        const int currentlyRunningBackgroundThreads = internal::_numberOfRunningBackgroundJobConsumerTasks;
        if (
          currentlyRunningBackgroundThreads<BackgroundJob::_maxNumberOfRunningBackgroundThreads
        ) {
          internal::BackgroundJobConsumerTask::enqueue();
        }
      }
      break;
    case BackgroundJobType::LongRunningBackgroundJob:
      {
        internal::_backgroundJobs.push(job);
        internal::BackgroundJobConsumerTask::enqueue();
      }
      break;
  }
}


/**
 * This routine is typically invoked by user codes to ensure that all
 * background jobs have finished before the user code continues. We have however
 * to take into account that some background jobs might reschedule themselves
 * again as they are persistent. Therefore, we quickly check how many jobs are
 * still pending. Then we add the number of running background jobs (as those
 * guys might reschedule themselves again, so we try to be on the same side).
 * Finally, we process that many jobs that are in the queue and tell the
 * calling routine whether we've done any.
 */
bool tarch::multicore::jobs::processBackgroundJobs() {
  const int numberOfBackgroundJobs = internal::_backgroundJobs.unsafe_size() + internal::_numberOfRunningBackgroundJobConsumerTasks + 1;
  return internal::processNumberOfBackgroundJobs(numberOfBackgroundJobs);
}


int tarch::multicore::jobs::getNumberOfWaitingBackgroundJobs() {
  return internal::_backgroundJobs.unsafe_size();
}


/**
 * Spawn means a thread fires a new job and wants to continue itself.
 *
 * <h2> The spawned job is a task </h2>
 *
 * That means that the new job has no dependencies on any other job. It is
 * thus convenient to launch a real TBB task for it.
 *
 * <h2> The spawned job is not a task </h2>
 *
 * We enqueue it. We may not immediately spawn a job consumer task, as this
 * might mean that TBB might immediately start to consume the job and halt the
 * current thread. This is not what we want: We want to continue with the
 * calling thread immediately.
 */
void tarch::multicore::jobs::spawn(Job*  job) {
  if ( job->isTask() ) {
    logDebug( "spawn(Job*)", "job is a task, so issue TBB task immediately that handles job" );
    internal::TBBJobWrapper* tbbTask = new(tbb::task::allocate_root(importantTaskContext)) internal::TBBJobWrapper(job);
    tbb::task::enqueue(*tbbTask);
  }
  else {
    internal::getJobQueue(job->getClass()).jobs.push(job);

    logDebug( "spawn(Job*)", "enqueued job of class " << job->getClass() );
  }
}


void tarch::multicore::jobs::spawn(std::function<void()>& job, bool isTask, int jobClass) {
  spawn( new tarch::multicore::jobs::GenericJobWithCopyOfFunctor(job,isTask,jobClass) );
}


/**
 * @see processJobs()
 */
int tarch::multicore::jobs::getNumberOfPendingJobs() {
  int result = 0;
  logDebug( "processJobs()", "there are " << _pendingJobs.size() << " class queues" );
  for (auto& p: internal::_pendingJobs) {
	result += p.second.jobs.unsafe_size();
  }
  return result;
}


bool tarch::multicore::jobs::processJobs(int jobClass) {
  logDebug( "processJobs()", "search for jobs of class " << jobClass );

  Job* myTask   = nullptr;
  bool gotOne   = internal::getJobQueue(jobClass).jobs.try_pop(myTask);
  bool result   = false;
  while (gotOne) {
    result   = true;
    logDebug( "processJob(int)", "start to process job of class " << jobClass );
    myTask->run();
    delete myTask;
    logDebug(
      "processJob(int)", "job of class " << jobClass << " complete, there are still " <<
	  getJobQueue(jobClass).jobs.unsafe_size() <<
	  " jobs of this class pending"
	);
    gotOne = internal::getJobQueue(jobClass).jobs.try_pop(myTask);
  }

  return result;
}


/**
 * Work way through the individual queues. Ensure that queues in turn do not 
 * invoke processJobs() again, i.e. pass in false as argument, as we otherwise
 * obtain endless cascadic recursion. The routine should not spawn new tasks on
 * its own, as it is itself used by the job consumer tasks.
 *
 * <h2> Implementation </h2>
 *
 * It is absolutely essential that one uses auto&. With a copy/read-only
 * reference, the code crashes if someone insert stuff concurrently.
 *
 * <h2> Danger </h2>
 *
 * If you run with many job classes, i.e. 'tasks' that depend on each other, then
 * invoking this routine is dangerous. It bears the risk that you spawn more and
 * more jobs that depend on another job and you thus run into a situation, where
 * all TBB tasks process one particular job type.
 */
bool tarch::multicore::jobs::processJobs() {
  bool result = false;

  for (auto& p: internal::_pendingJobs) {
	result |= processJobs(p.first);
  }
  
  return result;
}




/**
 * @see spawnBlockingJob
 */
void tarch::multicore::jobs::spawnAndWait(
  std::function<void()>&  job0,
  std::function<void()>&  job1,  
  bool                    isTask0,
  bool                    isTask1,
  int                     jobClass0,
  int                     jobClass1
) {

/*
  This version is serial and works
  --------------------------------

  job0();
  job1();
*/


/*
   This version is parallel and makes TBB crash
  ---------------------------------------------

   tbb::atomic<int>  semaphore(2);
  
  tbb::parallel_invoke(
    [&] () -> void {
	  internal::spawnBlockingJob( job0, semaphore, isTask0, jobClass0 );
    },
    [&] () -> void {
    	internal::spawnBlockingJob( job1, semaphore, isTask1, jobClass1 );
    }
  );
  while (semaphore>0) {
    tbb::parallel_invoke(
      [&] () -> void {
        processJobs(jobClass0);
      },
      [&] () -> void {
        processJobs(jobClass1);
      }
    );
  }*/

  tbb::atomic<int>  semaphore(2);
  tbb::task_group g;

  g.run( [&]() { internal::spawnBlockingJob( job0, semaphore, isTask0, jobClass0 ); });
  g.run( [&]() { internal::spawnBlockingJob( job1, semaphore, isTask1, jobClass1 ); });
  g.wait();

  while (semaphore>0) {
    g.run( [&]() { processJobs(jobClass0); });
    g.run( [&]() { processJobs(jobClass1); });
    g.wait();
  }
}



/**
 * @see spawnBlockingJob
 */
void tarch::multicore::jobs::spawnAndWait(
  std::function<void()>&  job0,
  std::function<void()>&  job1,
  std::function<void()>&  job2,
  bool                    isTask0,
  bool                    isTask1,
  bool                    isTask2,
  int                     jobClass0,
  int                     jobClass1,
  int                     jobClass2
) {
  tbb::atomic<int>  semaphore(3);
  tbb::task_group g;

  g.run( [&]() { internal::spawnBlockingJob( job0, semaphore, isTask0, jobClass0 ); });
  g.run( [&]() { internal::spawnBlockingJob( job1, semaphore, isTask1, jobClass1 ); });
  g.run( [&]() { internal::spawnBlockingJob( job2, semaphore, isTask2, jobClass2 ); });
  g.wait();

  #ifdef Asserts
  int deadlockCounter = 0;
  #endif
  while (semaphore>0) {
    g.run( [&]() { processJobs(jobClass0); });
    g.run( [&]() { processJobs(jobClass1); });
    g.run( [&]() { processJobs(jobClass2); });
    g.run( [&]() { internal::processNumberOfBackgroundJobs(1); });
    #ifdef Asserts
    deadlockCounter++;
    if (deadlockCounter>std::numeric_limits<int>::max()/2) {
      static tarch::logging::Log _log( "tarch::multicore::jobs" );
      logInfo( "spawnAndWait(...)", internal::report() );
      deadlockCounter = 0;
    }
    #endif
  }
  g.wait();
}


/**
 * @see spawnBlockingJob
 */
void tarch::multicore::jobs::spawnAndWait(
  std::function<void()>&  job0,
  std::function<void()>&  job1,
  std::function<void()>&  job2,
  std::function<void()>&  job3,
  bool                    isTask0,
  bool                    isTask1,
  bool                    isTask2,
  bool                    isTask3,
  int                     jobClass0,
  int                     jobClass1,
  int                     jobClass2,
  int                     jobClass3
) {
  tbb::atomic<int>  semaphore(3);
  tbb::task_group g;

  g.run( [&]() { internal::spawnBlockingJob( job0, semaphore, isTask0, jobClass0 ); });
  g.run( [&]() { internal::spawnBlockingJob( job1, semaphore, isTask1, jobClass1 ); });
  g.run( [&]() { internal::spawnBlockingJob( job2, semaphore, isTask2, jobClass2 ); });
  g.run( [&]() { internal::spawnBlockingJob( job3, semaphore, isTask3, jobClass3 ); });
  g.wait();

  #ifdef Asserts
  int deadlockCounter = 0;
  #endif
  while (semaphore>0) {
    g.run( [&]() { processJobs(jobClass0); });
    g.run( [&]() { processJobs(jobClass1); });
    g.run( [&]() { processJobs(jobClass2); });
    g.run( [&]() { processJobs(jobClass3); });
    g.run( [&]() { internal::processNumberOfBackgroundJobs(1); });
    #ifdef Asserts
    deadlockCounter++;
    if (deadlockCounter>std::numeric_limits<int>::max()/2) {
      static tarch::logging::Log _log( "tarch::multicore::jobs" );
      logInfo( "spawnAndWait(...)", internal::report() );
      deadlockCounter = 0;
    }
    #endif
  }
  g.wait();
}



/**
 * @see spawnBlockingJob
 */
void tarch::multicore::jobs::spawnAndWait(
  std::function<void()>&  job0,
  std::function<void()>&  job1,
  std::function<void()>&  job2,
  std::function<void()>&  job3,
  std::function<void()>&  job4,
  bool                    isTask0,
  bool                    isTask1,
  bool                    isTask2,
  bool                    isTask3,
  bool                    isTask4,
  int                     jobClass0,
  int                     jobClass1,
  int                     jobClass2,
  int                     jobClass3,
  int                     jobClass4
) {
  tbb::atomic<int>  semaphore(3);
  tbb::task_group g;

  g.run( [&]() { internal::spawnBlockingJob( job0, semaphore, isTask0, jobClass0 ); });
  g.run( [&]() { internal::spawnBlockingJob( job1, semaphore, isTask1, jobClass1 ); });
  g.run( [&]() { internal::spawnBlockingJob( job2, semaphore, isTask2, jobClass2 ); });
  g.run( [&]() { internal::spawnBlockingJob( job3, semaphore, isTask3, jobClass3 ); });
  g.run( [&]() { internal::spawnBlockingJob( job4, semaphore, isTask4, jobClass4 ); });
  g.wait();

  #ifdef Asserts
  int deadlockCounter = 0;
  #endif
  while (semaphore>0) {
    g.run( [&]() { processJobs(jobClass0); });
    g.run( [&]() { processJobs(jobClass1); });
    g.run( [&]() { processJobs(jobClass2); });
    g.run( [&]() { processJobs(jobClass3); });
    g.run( [&]() { processJobs(jobClass4); });
    g.run( [&]() { internal::processNumberOfBackgroundJobs(1); });
    #ifdef Asserts
    deadlockCounter++;
    if (deadlockCounter>std::numeric_limits<int>::max()/2) {
      static tarch::logging::Log _log( "tarch::multicore::jobs" );
      logInfo( "spawnAndWait(...)", internal::report() );
      deadlockCounter = 0;
    }
    #endif
  }
  g.wait();
}



/**
 * @see spawnBlockingJob
 */
void tarch::multicore::jobs::spawnAndWait(
  std::function<void()>&  job0,
  std::function<void()>&  job1,
  std::function<void()>&  job2,
  std::function<void()>&  job3,
  std::function<void()>&  job4,
  std::function<void()>&  job5,
  bool                    isTask0,
  bool                    isTask1,
  bool                    isTask2,
  bool                    isTask3,
  bool                    isTask4,
  bool                    isTask5,
  int                     jobClass0,
  int                     jobClass1,
  int                     jobClass2,
  int                     jobClass3,
  int                     jobClass4,
  int                     jobClass5
) {
  tbb::atomic<int>  semaphore(3);
  tbb::task_group g;

  g.run( [&]() { internal::spawnBlockingJob( job0, semaphore, isTask0, jobClass0 ); });
  g.run( [&]() { internal::spawnBlockingJob( job1, semaphore, isTask1, jobClass1 ); });
  g.run( [&]() { internal::spawnBlockingJob( job2, semaphore, isTask2, jobClass2 ); });
  g.run( [&]() { internal::spawnBlockingJob( job3, semaphore, isTask3, jobClass3 ); });
  g.run( [&]() { internal::spawnBlockingJob( job4, semaphore, isTask4, jobClass4 ); });
  g.run( [&]() { internal::spawnBlockingJob( job5, semaphore, isTask5, jobClass5 ); });
  g.wait();

  #ifdef Asserts
  int deadlockCounter = 0;
  #endif
  while (semaphore>0) {
    g.run( [&]() { processJobs(jobClass0); });
    g.run( [&]() { processJobs(jobClass1); });
    g.run( [&]() { processJobs(jobClass2); });
    g.run( [&]() { processJobs(jobClass3); });
    g.run( [&]() { processJobs(jobClass4); });
    g.run( [&]() { processJobs(jobClass5); });
    g.run( [&]() { internal::processNumberOfBackgroundJobs(1); });
    #ifdef Asserts
    deadlockCounter++;
    if (deadlockCounter>std::numeric_limits<int>::max()/2) {
      static tarch::logging::Log _log( "tarch::multicore::jobs" );
      logInfo( "spawnAndWait(...)", internal::report() );
      deadlockCounter = 0;
    }
    #endif
  }
  g.wait();
}


std::string tarch::multicore::jobs::internal::report() {
  std::ostringstream msg;
  msg << "job-system-status: queued-background-jobs=" << _backgroundJobs.unsafe_size()
	  << ", no-of-running-bg-consumer-tasks=" << _numberOfRunningBackgroundJobConsumerTasks;
  for (auto& p: _pendingJobs) {
	msg << ",queue[" << p.first
        << "]=" << p.second.jobs.unsafe_size() << " job(s)";
  }
  typedef tbb::concurrent_hash_map< int, JobQueue >  JobMap;
  extern JobMap     _pendingJobs;

  return msg.str();
}

#endif
