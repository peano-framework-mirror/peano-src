#include "tarch/multicore/Jobs.h"
#include "tarch/Assertions.h"


#include "tarch/multicore/MulticoreDefinitions.h"

#include <thread>



#if defined(SharedCPP)


#include <atomic>

#include "tarch/multicore/Jobs.h"


namespace {
  /**
   * The spawn and wait routines fire their job and then have to wait for all
   * jobs to be processed. They do this through an integer atomic that they
   * count down to zero, i.e. the atomic stores how many jobs are still
   * pending.
   */
  class JobWithoutCopyOfFunctorAndSemaphore: public tarch::multicore::jobs::Job {
    private:
      std::function<bool()>&   _functor;
      std::atomic<int>&        _semaphore;
    public:
      JobWithoutCopyOfFunctorAndSemaphore(std::function<bool()>& functor, tarch::multicore::jobs::JobType jobType, int jobClass, std::atomic<int>& semaphore ):
        Job(jobType,jobClass),
        _functor(functor),
        _semaphore(semaphore) {
      }

      bool run() override {
        bool result = _functor();
        if (!result) _semaphore.fetch_add(-1, std::memory_order_relaxed);
        return result;
      }

      virtual ~JobWithoutCopyOfFunctorAndSemaphore() {}
  };
}

#include "JobQueue.h"


void tarch::multicore::jobs::spawnBackgroundJob(Job* job) {
  switch (job->getJobType()) {
     case JobType::ProcessImmediately:
       while (job->run()) {};
       delete job;
       break;
     case JobType::RunTaskAsSoonAsPossible:
       internal::JobQueue::getBackgroundQueue().addJobWithHighPriority(job);
       break;
     case JobType::Task:
     case JobType::Job:
       internal::JobQueue::getBackgroundQueue().addJob(job);
       break;
   }
}


bool tarch::multicore::jobs::processBackgroundJobs() {
  return internal::JobQueue::getBackgroundQueue().processJobs( std::max(1,getNumberOfWaitingBackgroundJobs()/2) );
}


int tarch::multicore::jobs::getNumberOfWaitingBackgroundJobs() {
  return internal::JobQueue::getBackgroundQueue().getNumberOfPendingJobs();
}


void tarch::multicore::jobs::spawn(Job*  job) {
  if ( job->isTask() ) {
	internal::JobQueue::getStandardQueue(job->getClass()).addJobWithHighPriority(job);
  }
  else {
	internal::JobQueue::getStandardQueue(job->getClass()).addJob(job);
  }
}


void tarch::multicore::jobs::spawn(std::function<bool()>& job, JobType jobType, int jobClass) {
  spawn( new tarch::multicore::jobs::GenericJobWithCopyOfFunctor(job,jobType,jobClass) );
}


int tarch::multicore::jobs::getNumberOfPendingJobs() {
  int result = 0;
  for (int i=0; i<internal::JobQueue::MaxNormalJobQueues; i++) {
    result += internal::JobQueue::getStandardQueue(i).getNumberOfPendingJobs();
  }
  return result;
}


bool tarch::multicore::jobs::processJobs(int jobClass, int maxNumberOfJobs) {
  if (internal::JobQueue::getStandardQueue(jobClass).getNumberOfPendingJobs()>0) {
    return internal::JobQueue::getStandardQueue(jobClass).processJobs(maxNumberOfJobs);
  }
  else return false;
}


/*
void processAllJobs() {
  for( int i=0; i<tarch::multicore::internal::JobQueue::MaxNormalJobQueues; i++) {
    if (tarch::multicore::internal::JobQueue::getStandardQueue(i).getNumberOfPendingJobs()>0) {
      tarch::multicore::internal::JobQueue::getStandardQueue(i).processJobs(1);
    }
  }
}
*/


void tarch::multicore::jobs::spawnAndWait(
  std::function<bool()>&  job0,
  std::function<bool()>&  job1,
  JobType                 jobType0,
  JobType                 jobType1,
  int                     jobClass0,
  int                     jobClass1
) {
  std::atomic<int>  semaphore(2);

  internal::JobQueue::getStandardQueue(jobClass0).addJob( new JobWithoutCopyOfFunctorAndSemaphore(job0, jobType0, jobClass0, semaphore ) );
  internal::JobQueue::getStandardQueue(jobClass0).addJob( new JobWithoutCopyOfFunctorAndSemaphore(job1, jobType1, jobClass1, semaphore ) );

  while (semaphore.load()!=0) {
    processJobs(jobClass0,1);
    processJobs(jobClass1,1);
  }
}


void tarch::multicore::jobs::spawnAndWait(
  std::function<bool()>&  job0,
  std::function<bool()>&  job1,
  std::function<bool()>&  job2,
  JobType                 jobType0,
  JobType                 jobType1,
  JobType                 jobType2,
  int                     jobClass0,
  int                     jobClass1,
  int                     jobClass2
) {
  std::atomic<int>  semaphore(3);

  internal::JobQueue::getStandardQueue(jobClass0).addJob( new JobWithoutCopyOfFunctorAndSemaphore(job0, jobType0, jobClass0, semaphore ) );
  internal::JobQueue::getStandardQueue(jobClass0).addJob( new JobWithoutCopyOfFunctorAndSemaphore(job1, jobType1, jobClass1, semaphore ) );
  internal::JobQueue::getStandardQueue(jobClass0).addJob( new JobWithoutCopyOfFunctorAndSemaphore(job2, jobType2, jobClass2, semaphore ) );

  while (semaphore.load()!=0) {
    processJobs(jobClass0,1);
    processJobs(jobClass1,1);
    processJobs(jobClass2,1);
  }
}


void tarch::multicore::jobs::spawnAndWait(
  std::function<bool()>& job0,
  std::function<bool()>& job1,
  std::function<bool()>& job2,
  std::function<bool()>& job3,
  JobType                    jobType0,
  JobType                    jobType1,
  JobType                    jobType2,
  JobType                    jobType3,
  int                        jobClass0,
  int                        jobClass1,
  int                        jobClass2,
  int                        jobClass3
) {
  std::atomic<int>  semaphore(4);

  internal::JobQueue::getStandardQueue(jobClass0).addJob( new JobWithoutCopyOfFunctorAndSemaphore(job0, jobType0, jobClass0, semaphore ) );
  internal::JobQueue::getStandardQueue(jobClass0).addJob( new JobWithoutCopyOfFunctorAndSemaphore(job1, jobType1, jobClass1, semaphore ) );
  internal::JobQueue::getStandardQueue(jobClass0).addJob( new JobWithoutCopyOfFunctorAndSemaphore(job2, jobType2, jobClass2, semaphore ) );
  internal::JobQueue::getStandardQueue(jobClass0).addJob( new JobWithoutCopyOfFunctorAndSemaphore(job3, jobType3, jobClass3, semaphore ) );

  while (semaphore.load()!=0) {
    processJobs(jobClass0,1);
    processJobs(jobClass1,1);
    processJobs(jobClass2,1);
    processJobs(jobClass3,1);
  }
}


void tarch::multicore::jobs::spawnAndWait(
  std::function<bool()>& job0,
  std::function<bool()>& job1,
  std::function<bool()>& job2,
  std::function<bool()>& job3,
  std::function<bool()>& job4,
  JobType                    jobType0,
	 JobType                    jobType1,
	 JobType                    jobType2,
	 JobType                    jobType3,
	 JobType                    jobType4,
	 int                     jobClass0,
	 int                     jobClass1,
	 int                     jobClass2,
	 int                     jobClass3,
	 int                     jobClass4
) {
  std::atomic<int>  semaphore(5);

  internal::JobQueue::getStandardQueue(jobClass0).addJob( new JobWithoutCopyOfFunctorAndSemaphore(job0, jobType0, jobClass0, semaphore ) );
  internal::JobQueue::getStandardQueue(jobClass0).addJob( new JobWithoutCopyOfFunctorAndSemaphore(job1, jobType1, jobClass1, semaphore ) );
  internal::JobQueue::getStandardQueue(jobClass0).addJob( new JobWithoutCopyOfFunctorAndSemaphore(job2, jobType2, jobClass2, semaphore ) );
  internal::JobQueue::getStandardQueue(jobClass0).addJob( new JobWithoutCopyOfFunctorAndSemaphore(job3, jobType3, jobClass3, semaphore ) );
  internal::JobQueue::getStandardQueue(jobClass0).addJob( new JobWithoutCopyOfFunctorAndSemaphore(job4, jobType4, jobClass4, semaphore ) );

  while (semaphore.load()!=0) {
    processJobs(jobClass0,1);
    processJobs(jobClass1,1);
    processJobs(jobClass2,1);
    processJobs(jobClass3,1);
    processJobs(jobClass4,1);
  }
}


void tarch::multicore::jobs::spawnAndWait(
  std::function<bool()>& job0,
  std::function<bool()>& job1,
  std::function<bool()>& job2,
  std::function<bool()>& job3,
  std::function<bool()>& job4,
  std::function<bool()>& job5,
  JobType                    jobType0,
  JobType                    jobType1,
  JobType                    jobType2,
  JobType                    jobType3,
  JobType                    jobType4,
  JobType                    jobType5,
  int                     jobClass0,
  int                     jobClass1,
  int                     jobClass2,
  int                     jobClass3,
  int                     jobClass4,
  int                     jobClass5
) {
  std::atomic<int>  semaphore(6);

  internal::JobQueue::getStandardQueue(jobClass0).addJob( new JobWithoutCopyOfFunctorAndSemaphore(job0, jobType0, jobClass0, semaphore ) );
  internal::JobQueue::getStandardQueue(jobClass0).addJob( new JobWithoutCopyOfFunctorAndSemaphore(job1, jobType1, jobClass1, semaphore ) );
  internal::JobQueue::getStandardQueue(jobClass0).addJob( new JobWithoutCopyOfFunctorAndSemaphore(job2, jobType2, jobClass2, semaphore ) );
  internal::JobQueue::getStandardQueue(jobClass0).addJob( new JobWithoutCopyOfFunctorAndSemaphore(job3, jobType3, jobClass3, semaphore ) );
  internal::JobQueue::getStandardQueue(jobClass0).addJob( new JobWithoutCopyOfFunctorAndSemaphore(job4, jobType4, jobClass4, semaphore ) );
  internal::JobQueue::getStandardQueue(jobClass0).addJob( new JobWithoutCopyOfFunctorAndSemaphore(job5, jobType5, jobClass5, semaphore ) );

  while (semaphore.load()!=0) {
    processJobs(jobClass0,1);
    processJobs(jobClass1,1);
    processJobs(jobClass2,1);
    processJobs(jobClass3,1);
    processJobs(jobClass4,1);
    processJobs(jobClass5,1);
  }
}


#endif

