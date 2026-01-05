#pragma once

#include <atomic>
#include <chrono>
#include <mutex>
#include <memory>
#include <vector>
#include <functional>

#include "job_manager.h"
#include "thread_worker.h"

class job_manager;
class thread_pool: public std::enable_shared_from_this<thread_pool>
{
public:
	thread_pool();
	virtual ~thread_pool();

public:
	std::shared_ptr<thread_pool> getPtr(void);

public:
	void addWorker(std::shared_ptr<thread_worker> new_worker);
	void removeWorker(std::shared_ptr<thread_worker> worker);
	void removeWorkers();

public:
	std::map<job_priority, int> _priority_worker_numbers;
	void setWorkersPriorityNumbers();
	int getWorkerNumbers();

public:
	void addJob(std::shared_ptr<job> new_job);

public:
	void stopPool(bool wait_for_finish_jobs = false, std::chrono::seconds max_wait_time = std::chrono::seconds(0));

public:
	std::weak_ptr<job_manager> getJobManager();

private:
	std::mutex _woker_mutex;

	std::atomic_bool _terminated;

	std::shared_ptr<job_manager> _job_manager;
	std::vector<std::shared_ptr<thread_worker>> _workers;

public:
	void notifyWakeUpWorkers();

};

