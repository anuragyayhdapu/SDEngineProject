#pragma once

#include <thread>
#include <atomic>
#include <unordered_set>
#include <vector>
#include <deque>
#include <mutex>


class JobSystem;


enum class JobType
{
	DISK_IO,
	COMPUTATION
};


//-------------------------------------------------------------------------
// Abstract Base Class; 
class Job
{
public:
	Job() = default;
	virtual ~Job() = default;
	virtual void Execute() = 0;

	JobType GetType() const { return m_type; }

protected:
	std::atomic<JobType> m_type = JobType::COMPUTATION;
};


//-------------------------------------------------------------------------
class JobWorkerThread
{
public:
	JobWorkerThread(JobSystem* jobSystem, int threadId, JobType workingJobType);
	~JobWorkerThread();

	void ThreadMain();

	JobSystem* m_jobSystem = nullptr;
	int m_threadID = -1;
	std::thread* m_thread = nullptr;

	JobType m_workingJobType = JobType::COMPUTATION;
};


struct JobSystemConfig
{
	int m_preferredNumberOfWorkerThreads = -1; // -1 means "one fewer than num of Cores"
};

//-------------------------------------------------------------------------
class JobSystem
{
	friend class JobWorker;

public:
	JobSystem(JobSystemConfig config);


	void Startup();
	void BeginFrame();
	void EndFrame();
	void Shutdown();


	void PostNewJob(Job* job);							// called by Main Thread to add a job to the queue
	Job* RetriveOneCompletedJob();						// called by Main Thread to get a job that has been completed
	std::unordered_set<Job*> RetrieveAllCompleteJobs(); // called by Main Thread to get all jobs that have been completed
	std::unordered_set<Job*> RetrieveAllCompletedJobsOfType(JobType type);


	bool IsQuitting();

	Job* GetNewJobToWorkOn(JobType jobType);
	void MarkJobAsComplete(Job* job);

	JobSystemConfig m_config;

private:
	std::vector<JobWorkerThread*> m_workers;

	std::deque<Job*> m_unclaimedJobsQueue;		 // the jobs that have been posted by the main thread
	std::mutex		 m_unclaimedJobsMutex;		// mutex that allows access to the jobs list

	std::unordered_set<Job*> m_claimedJobsSet;	// List of jobs currently claimed by workers
	std::mutex		  m_claimedJobsMutex;

	std::unordered_set<Job*> m_completedJobsSet; // List of Jobs finished, ready yo be retrieved
	std::mutex		  m_completedJobsMutex;

	std::atomic<bool> m_isQuitting = false;

	void CreateWorkers(int numWorkerThreads, JobType workingJobType, int startingIndex);
};