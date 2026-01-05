#pragma once

#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "job_manager.h"

class thread_worker
{
public:
	thread_worker(job_priority job_priority = job_priority::NORMAL_PRIORITY);
	~thread_worker();

	void setJobManager(std::shared_ptr<job_manager> job_manager);

private:
	job_priority _job_priority;
	std::vector<job_priority> _job_match_priorities;
	std::atomic_bool _terminated;

	std::jthread _worker_thread;
	std::mutex _worker_mutex;
	std::condition_variable_any _worker_condition;

	std::weak_ptr<job_manager> _job_manager;

private:
	void jobCountChanged();
	bool checkwakeUpCondition();

public:
	void startWorker();
	void stopWorker();

	job_priority getPriority();
	void setJobMatchPriorities();

public:
	void notifyWakeUp();
	void worker_function(std::stop_token stop_token);
};

