// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _TARCH_MULTICORE_JOBS_H_
#define _TARCH_MULTICORE_JOBS_H_


#include <functional>
#include <limits>


namespace tarch {
  namespace multicore {
    /**
     * Jobs are Peano's abstraction of tasks. They generalise the
     * term tasks. A task in Peano's notion follows Intel's TBB and is an
     * atomic unit. That is, it may have children which have to be processed
     * first, but it can not be interrupted and it does not depend on anybody
     * besides its children. Notably, the only way to interrupt a task is to
     * spawn children which implies that the task stops immediately, waits
     * for the children and then returns. Peano's jobs are a generalisation:
     * they may depend on other jobs. If they do not, a job is equivalent to
     * a task.
     */
    namespace jobs {
       enum class JobType {
         Job,
         Task,
		 MPIReceiveTask,
		 /**
		  * Task implies that there are no dependencies.
		  */
		 RunTaskAsSoonAsPossible,
		 /**
		  * It does not really make sense to specify this flag by a user.
		  * But it is used internally if background threads are disabled.
		  */
		 ProcessImmediately
       };

       /**
        * Effectively switches off all background tasks. This could lead to
        * deadlocks!
        */
       constexpr int DontUseAnyBackgroundJobs            = -1;

       /**
        * Abstract super class for a job. Job class is an integer. A job may
        * depend on input data from other jobs and may write out data to
        * another job as well (through a shared memory region protected by
        * a semaphore). However, it should never exchange information with
        * another job of the same class.
        */
       class Job {
         private:
           const JobType  _jobType;
    	   const int      _jobClass;

    	   friend void spawnBackgroundJob(Job* job);
    	   friend bool processBackgroundJobs();

    	   static int _maxNumberOfRunningBackgroundThreads;
         public:
    	   /**
    	    * A task is a job without any dependencies on other jobs though it
    	    * might have children. Tasks form a tree structure. Jobs may form
    	    * a DAG.
    	    */
    	   Job( JobType jobType, int jobClass );

           virtual bool run() = 0;
           virtual ~Job();
           bool isTask() const;
           int getClass() const;
           JobType getJobType() const;

           /**
            * If jobs are enqueued, they are typically not processed
            * immediately (unless we run on a single core machine), but Peano
            * puts them into a queue. This queue then is processed by
            * tasks/threads/whatever they are called. The maximum nmber of
            * these guys that work themselves through the queue of jobs is
            * set by this routine. By default, there is no upper number, so
            * you might end up with all your cores doing background jobs.
            *
            * @param maxNumberOfRunningBackgroundThreads -1 Switch off any tasks
            *   running in the background.
            * @param maxNumberOfRunningBackgroundThreads 0 Do not use background tasks
            *   unless the user instructs the component that this background task is a
            *   very long running task. If a long-lasting task is issued, the component
            *   launches a task for it specifically.
            * @param maxNumberOfRunningBackgroundThreads Usually, 1 or any small
            *   number should be sufficient. Small is to be read relative to the
            *   threads available. You don't want your system to spend all of its tasks
            *   onto background activities at any time. You can also use the constants.
            *
            * @see DontUseAnyBackgroundJobs
            * @see ProcessNormalBackgroundJobsImmediately
            */
           static void setMaxNumberOfRunningBackgroundThreads(int maxNumberOfRunningBackgroundThreads);
       };

       /**
        * Frequently used implementation for job with a functor.
        */
       class GenericJobWithCopyOfFunctor: public Job {
         private:
    	   /**
            * See the outer class description for an explanation why this is an
            * attribute, i.e. why we copy the functor here always.
            */
    	   std::function<bool()>   _functor;
         public:
           GenericJobWithCopyOfFunctor( const std::function<bool()>& functor, JobType jobType, int jobClass );

           bool run() override;

           virtual ~GenericJobWithCopyOfFunctor();
       };

       /**
        * Frequently used implementation for job with a functor.
        */
       class GenericJobWithoutCopyOfFunctor: public Job {
         private:
    	   /**
            * See the outer class description for an explanation why this is an
            * attribute, i.e. why we copy the functor here always.
            */
    	   std::function<bool()>&   _functor;
         public:
           GenericJobWithoutCopyOfFunctor(std::function<bool()>& functor, JobType jobType, int jobClass );

           bool run() override;

           virtual ~GenericJobWithoutCopyOfFunctor();
       };


       template <typename T>
       class GenericJobWithPointer: public Job {
         private:
    	   /**
            * See the outer class description for an explanation why this is an
            * attribute, i.e. why we copy the functor here always.
            */
    	   T*   _functor;
         public:
    	   GenericJobWithPointer(T* functor, JobType jobType, int jobClass  ):
             Job(jobType,jobClass),
             _functor(functor)  {
    	   }


           bool run() override {
             return (*_functor)();
           }

           virtual ~GenericJobWithPointer() {
        	 delete _functor;
           }
       };

       /**
        * Ownership for pointer goes to multicore component, i.e. the multicore
        * component deletes the task once it has finished.
        */
       void spawnBackgroundJob(Job* task);

       /**
        * Work through the background tasks and let the caller know whether some
        * tasks have been processed.
        */
       bool processBackgroundJobs();

       /**
        * This is the logical number of background tasks, i.e. how many things
        * could, in theory, run the the background.
        */
       int getNumberOfWaitingBackgroundJobs();

       /**
        * Kick out a new job. The job's type has to be set properly: It
        * has to be clear whether the job is a job or even a task, i.e. a
        * special type of job. In the latter case, a runtime system could
        * deploy the pointer to a different thread and return immediately.
        *
        * Ownership goes over to Peano's job namespace, i.e. you don't have
        * to delete the pointer.
        */
       void spawn(Job*  job);

       /**
        * Wrapper around other spawn operation.
        */
       void spawn(std::function<bool()>& job, JobType jobType, int jobClass);

       void spawnAndWait(
         std::function<bool()>&  job0,
         std::function<bool()>&  job1,
		 JobType                 jobType0,
		 JobType                 jobType1,
		 int                     jobClass0,
		 int                     jobClass1
       );

       void spawnAndWait(
         std::function<bool()>&  job0,
         std::function<bool()>&  job1,
         std::function<bool()>&  job2,
		 JobType                 jobType0,
		 JobType                 jobType1,
		 JobType                 jobType2,
		 int                     jobClass0,
		 int                     jobClass1,
		 int                     jobClass2
       );

       void spawnAndWait(
         std::function<bool()>&  job0,
         std::function<bool()>&  job1,
         std::function<bool()>&  job2,
         std::function<bool()>&  job3,
		 JobType                 jobType0,
		 JobType                 jobType1,
		 JobType                 jobType2,
		 JobType                 jobType3,
		 int                     jobClass0,
		 int                     jobClass1,
		 int                     jobClass2,
		 int                     jobClass3
       );

       void spawnAndWait(
         std::function<bool()>&  job0,
         std::function<bool()>&  job1,
         std::function<bool()>&  job2,
         std::function<bool()>&  job3,
         std::function<bool()>&  job4,
		 JobType                 jobType0,
		 JobType                 jobType1,
		 JobType                 jobType2,
		 JobType                 jobType3,
		 JobType                 jobType4,
		 int                     jobClass0,
		 int                     jobClass1,
		 int                     jobClass2,
		 int                     jobClass3,
		 int                     jobClass4
       );

       void spawnAndWait(
         std::function<bool()>&  job0,
         std::function<bool()>&  job1,
         std::function<bool()>&  job2,
         std::function<bool()>&  job3,
         std::function<bool()>&  job4,
         std::function<bool()>&  job5,
		 JobType                 jobType0,
		 JobType                 jobType1,
		 JobType                 jobType2,
		 JobType                 jobType3,
		 JobType                 jobType4,
		 JobType                 jobType5,
		 int                     jobClass0,
		 int                     jobClass1,
		 int                     jobClass2,
		 int                     jobClass3,
		 int                     jobClass4,
		 int                     jobClass5
       );

       int getNumberOfPendingJobs();

       /**
        * Handle only jobs of one job class.
        */
       bool processJobs(int jobClass, int maxNumberOfJobs = std::numeric_limits<int>::max() );
    }
  }
}

#endif

