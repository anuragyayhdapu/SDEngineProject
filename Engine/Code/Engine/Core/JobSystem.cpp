#include "Engine/Core/JobSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"


//-------------------------------------------------------------------------
JobWorkerThread::JobWorkerThread(JobSystem* jobSystem, int threadId, JobType workingJobType) :
	m_jobSystem(jobSystem),
	m_threadID(threadId),
	m_workingJobType(workingJobType)
{
	m_thread = new std::thread(&JobWorkerThread::ThreadMain, this);
}

JobWorkerThread::~JobWorkerThread()
{
	delete m_thread;
}

void JobWorkerThread::ThreadMain()
{
	while (!m_jobSystem->IsQuitting())
	{
		Job* jobToDo = m_jobSystem->GetNewJobToWorkOn(m_workingJobType);

		if (jobToDo)
		{
			jobToDo->Execute(); // this will take some time to complete

			m_jobSystem->MarkJobAsComplete(jobToDo);
		}
		else
		{
			// sleep
			// don't hog the CPU, just checking for work
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}
}




//-------------------------------------------------------------------------
JobSystem::JobSystem(JobSystemConfig config) : m_config(config)
{
}

void JobSystem::Startup()
{
	int numWorkers = m_config.m_preferredNumberOfWorkerThreads;

	if (numWorkers < 0)
	{
		int numCpuCores = std::thread::hardware_concurrency();
		numWorkers = numCpuCores - 1;
	}

	int diskIOWorkeres = NUM_DISK_IO_THREADS;
	int computationWorkers = numWorkers - diskIOWorkeres;

	CreateWorkers(diskIOWorkeres, JobType::DISK_IO, NUM_DISK_IO_THREADS - 1);
	CreateWorkers(computationWorkers, JobType::COMPUTATION, NUM_DISK_IO_THREADS);
}

void JobSystem::BeginFrame()
{
}

void JobSystem::EndFrame()
{
}

void JobSystem::Shutdown()
{
	m_isQuitting = true;

	// join all threads
	for (int index = 0; index < m_workers.size(); ++index)
	{
		JobWorkerThread* worker = m_workers[index];
		worker->m_thread->join();

		delete worker;
		m_workers[index] = nullptr;
	}

	// flush and delete all unclaimed jobs
	while (!m_unclaimedJobsQueue.empty())
	{
		Job* job = m_unclaimedJobsQueue.front();
		m_unclaimedJobsQueue.pop_front();
		delete job;
	}

	// flush and delete all claimed jobs
	for (auto iter = m_completedJobsSet.begin(); iter != m_completedJobsSet.end(); iter++)
	{
		Job* job = *iter;
		delete job;
	}
	m_completedJobsSet.clear();

	// flush and delete all claimed jobs
	for (auto iter = m_claimedJobsSet.begin(); iter != m_claimedJobsSet.end(); iter++)
	{
		Job* job = *iter;
		delete job;
	}
	m_claimedJobsSet.clear();
}


//-------------------------------------------------------------------------
void JobSystem::CreateWorkers(int numWorkerThreads, JobType workingJobType, int startingIndex)
{
	for (int index = startingIndex; index < numWorkerThreads; ++index)
	{
		JobWorkerThread* worker = new JobWorkerThread(this, index, workingJobType);
		m_workers.push_back(worker);
	}
}

void JobSystem::PostNewJob(Job* job)
{
	// add a new job to unclaimed list
	//-------------------------------------------------------------------------
	// lock
	m_unclaimedJobsMutex.lock();
	m_unclaimedJobsQueue.push_back(job);
	m_unclaimedJobsMutex.unlock();
	// unlock
	//-------------------------------------------------------------------------
}


Job* JobSystem::RetriveOneCompletedJob()
{
	Job* completedJob = nullptr;

	//-------------------------------------------------------------------------
	// lock
	m_completedJobsMutex.lock();
	if (!m_completedJobsSet.empty())
	{
		completedJob = *m_completedJobsSet.begin();
		m_completedJobsSet.erase(m_completedJobsSet.begin());
	}
	m_completedJobsMutex.unlock();
	// unlock
	//-------------------------------------------------------------------------

	return completedJob;
}


std::unordered_set<Job*> JobSystem::RetrieveAllCompleteJobs()
{
	std::unordered_set<Job*> completedJobs;

	//-------------------------------------------------------------------------
	// lock
	m_completedJobsMutex.lock();

	completedJobs = m_completedJobsSet;
	m_completedJobsSet.clear();

	m_completedJobsMutex.unlock();
	// unlock
	//-------------------------------------------------------------------------

	return completedJobs;
}


std::unordered_set<Job*> JobSystem::RetrieveAllCompletedJobsOfType(JobType type)
{
	std::unordered_set<Job*> completedJobsOfType;

	//-------------------------------------------------------------------------
	// lock
	m_completedJobsMutex.lock();

	for (auto iter = m_completedJobsSet.begin(); iter != m_completedJobsSet.end(); iter++)
	{
		Job* job = *iter;
		if (job->GetType() == type)
		{
			completedJobsOfType.insert(job);
		}
	}

	for (auto iter = completedJobsOfType.begin(); iter != completedJobsOfType.end(); iter++)
	{
		Job* job = *iter;
		m_completedJobsSet.erase(job);
	}

	m_completedJobsMutex.unlock();
	// unlock
	//-------------------------------------------------------------------------

	return completedJobsOfType;
}


bool JobSystem::IsQuitting()
{
	return m_isQuitting;
}

Job* JobSystem::GetNewJobToWorkOn(JobType jobType)
{
	Job* jobToDo = nullptr;

	// get job from un-claimed list
	//-------------------------------------------------------------------------
	// lock
	m_unclaimedJobsMutex.lock();

	for (auto iter = m_unclaimedJobsQueue.begin(); iter != m_unclaimedJobsQueue.end(); iter++)
	{
		Job* job = *iter;
		if (job->GetType() == jobType)
		{
			jobToDo = job;
			m_unclaimedJobsQueue.erase(iter);
			break;
		}
	}

	m_unclaimedJobsMutex.unlock();
	// unlock
	//-------------------------------------------------------------------------


	// move to claimed set
	if (jobToDo)
	{
		//-------------------------------------------------------------------------
		// lock
		m_claimedJobsMutex.lock();
		m_claimedJobsSet.insert(jobToDo);
		m_claimedJobsMutex.unlock();
		// unlock
		//-------------------------------------------------------------------------
	}

	return jobToDo;
}

void JobSystem::MarkJobAsComplete(Job* job)
{
	// removed from claimed set
	//-------------------------------------------------------------------------
	// lock
	m_claimedJobsMutex.lock();
	m_claimedJobsSet.erase(job);
	m_claimedJobsMutex.unlock();
	//unlock
	//-------------------------------------------------------------------------


	// move completed job to claimed list
	//-------------------------------------------------------------------------
	// lock
	m_completedJobsMutex.lock();
	m_completedJobsSet.insert(job);
	m_completedJobsMutex.unlock();
	// unlock
	//-------------------------------------------------------------------------
}

