#include "thread_pool.h"

thread_pool::thread_pool()
	: _terminated(false)
{
	_job_manager = std::make_shared<job_manager>();
	_job_manager->setWorkerNotification(std::bind(&thread_pool::notifyWakeUpWorkers, this));
}

thread_pool::~thread_pool()
{
}

std::shared_ptr<thread_pool> thread_pool::getPtr(void)
{
	return shared_from_this();
}

void thread_pool::addWorker(std::shared_ptr<thread_worker> new_worker)
{
	if (_terminated)
	{
		return;
	}

	std::lock_guard<std::mutex> locker(_woker_mutex);

	std::vector<std::shared_ptr<thread_worker>>::iterator it = std::find(_workers.begin(), _workers.end(), new_worker);

	if (it != _workers.end())
	{
		return;
	}

	_workers.push_back(new_worker);

	new_worker->setJobManager(_job_manager);
	new_worker->startWorker();
}

void thread_pool::removeWorker(std::shared_ptr<thread_worker> worker)
{
	std::lock_guard<std::mutex> locker(_woker_mutex);

	std::vector<std::shared_ptr<thread_worker>>::iterator it = std::find(_workers.begin(), _workers.end(), worker);

	if (it == _workers.end())
	{
		return;
	}

	_workers.erase(it);
}

void thread_pool::removeWorkers()
{
	std::lock_guard<std::mutex> locker(_woker_mutex);

	for (int i = 0; i < (int)_workers.size(); i++)
	{
		_workers[i].reset();
	}

	_workers.clear();
}

void thread_pool::setWorkersPriorityNumbers()
{
	std::lock_guard<std::mutex> locker(_woker_mutex);

	for (int i = 0; i < (int)_workers.size(); i++)
	{
		auto iter = _priority_worker_numbers.find(_workers[i]->getPriority());

		if (iter == _priority_worker_numbers.end())
		{
			_priority_worker_numbers.insert({ _workers[i]->getPriority() , 0 });
			iter = _priority_worker_numbers.find(_workers[i]->getPriority());
		}

		iter->second++;
	}
}

int thread_pool::getWorkerNumbers()
{
	std::lock_guard<std::mutex> locker(_woker_mutex);

	return (int)_workers.size();
}

void thread_pool::addJob(std::shared_ptr<job> new_job)
{
	if (_terminated)
	{
		return;
	}

	// if there is no worker can do new_job's priority(HIGH or LOW), change to NORMAL_PRIORITY
	job_priority new_job_priority = new_job->getJobPriority();

	if (new_job_priority == job_priority::HIGH_PRIORITY || new_job_priority == job_priority::LOW_PRIORITY)
	{
		auto iter = _priority_worker_numbers.find(new_job_priority);

		if (iter == _priority_worker_numbers.end())
		{
			new_job->setJobPriority(job_priority::NORMAL_PRIORITY);
		}
		else
		{
			if (iter->second <= 0)
			{
				new_job->setJobPriority(job_priority::NORMAL_PRIORITY);
			}
		}
	}

	_job_manager->push_job(new_job);
}

void thread_pool::stopPool(bool wait_for_finish_jobs, std::chrono::seconds max_wait_time)
{
	if (wait_for_finish_jobs)
	{
		auto start_time = std::chrono::steady_clock::now();
		while (_job_manager->getAllJobCount() > 0)
		{
			if (max_wait_time != std::chrono::seconds(0) && std::chrono::steady_clock::now() - start_time >= max_wait_time)
			{
				break;
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}

	std::lock_guard<std::mutex> locker(_woker_mutex);

	for (int i = 0; i < (int)_workers.size(); i++)
	{
		if (_workers[i] != nullptr)
		{
			_workers[i]->stopWorker();
		}
	}

	_workers.clear();
}

std::weak_ptr<job_manager> thread_pool::getJobManager()
{
	return _job_manager;
}

void thread_pool::notifyWakeUpWorkers()
{
	std::lock_guard<std::mutex> locker(_woker_mutex);

	for (int i = 0; i < (int)_workers.size(); i++)
	{
		if (_workers[i] != nullptr)
		{
			_workers[i]->notifyWakeUp();
		}
	}
}
