#include "thread_pool.h"

thread_pool::thread_pool()
	: _terminated(false)
{
	this->_job_manager = std::make_shared<job_manager>();
	this->_job_manager->setWorkerNotification(std::bind(&thread_pool::notifyWakeUpWorkers, this));
}

thread_pool::~thread_pool()
{
}

std::shared_ptr<thread_pool> thread_pool::getPtr(void)
{
	return this->shared_from_this();
}

void thread_pool::addWorker(std::shared_ptr<thread_worker> new_worker)
{
	if (this->_terminated)
	{
		return;
	}

	std::lock_guard<std::mutex> locker(this->_woker_mutex);

	std::vector<std::shared_ptr<thread_worker>>::iterator it = std::find(this->_workers.begin(), this->_workers.end(), new_worker);

	if (it != this->_workers.end())
	{
		return;
	}

	this->_workers.push_back(new_worker);

	new_worker->setJobManager(this->_job_manager);
	new_worker->startWorker();
}

void thread_pool::removeWorker(std::shared_ptr<thread_worker> worker)
{
	std::lock_guard<std::mutex> locker(this->_woker_mutex);

	std::vector<std::shared_ptr<thread_worker>>::iterator it = std::find(this->_workers.begin(), this->_workers.end(), worker);

	if (it == this->_workers.end())
	{
		return;
	}

	this->_workers.erase(it);
}

void thread_pool::removeWorkers()
{
	std::lock_guard<std::mutex> locker(this->_woker_mutex);

	for (int i = 0; i < (int)this->_workers.size(); i++)
	{
		this->_workers[i].reset();
	}

	this->_workers.clear();
}

void thread_pool::setWorkersPriorityNumbers()
{
	std::lock_guard<std::mutex> locker(this->_woker_mutex);

	for (int i = 0; i < (int)this->_workers.size(); i++)
	{
		auto iter = this->_priority_worker_numbers.find(this->_workers[i]->getPriority());

		if (iter == this->_priority_worker_numbers.end())
		{
			this->_priority_worker_numbers.insert({ this->_workers[i]->getPriority() , 0 });
			iter = this->_priority_worker_numbers.find(this->_workers[i]->getPriority());
		}

		iter->second++;
	}
}

int thread_pool::getWorkerNumbers()
{
	std::lock_guard<std::mutex> locker(this->_woker_mutex);

	return (int)this->_workers.size();
}

void thread_pool::addJob(std::shared_ptr<job> new_job)
{
	if (this->_terminated)
	{
		return;
	}

	// if there is no worker can do new_job's priority(HIGH or LOW), change to NORMAL_PRIORITY
	job_priority new_job_priority = new_job->getJobPriority();

	if (new_job_priority == job_priority::HIGH_PRIORITY || new_job_priority == job_priority::LOW_PRIORITY)
	{
		auto iter = this->_priority_worker_numbers.find(new_job_priority);

		if (iter == this->_priority_worker_numbers.end())
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

	this->_job_manager->push_job(new_job);
}

void thread_pool::stopPool(bool wait_for_finish_jobs, std::chrono::seconds max_wait_time)
{
	if (wait_for_finish_jobs)
	{
		auto start_time = std::chrono::steady_clock::now();
		while (this->_job_manager->getAllJobCount() > 0)
		{
			if (max_wait_time != std::chrono::seconds(0) && std::chrono::steady_clock::now() - start_time >= max_wait_time)
			{
				break;
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}

	std::lock_guard<std::mutex> locker(this->_woker_mutex);

	for (int i = 0; i < (int)this->_workers.size(); i++)
	{
		if (this->_workers[i] != nullptr)
		{
			this->_workers[i]->stopWorker();
		}
	}

	this->_workers.clear();
}

std::weak_ptr<job_manager> thread_pool::getJobManager()
{
	return this->_job_manager;
}

void thread_pool::notifyWakeUpWorkers()
{
	std::lock_guard<std::mutex> locker(this->_woker_mutex);

	for (int i = 0; i < (int)this->_workers.size(); i++)
	{
		if (this->_workers[i] != nullptr)
		{
			this->_workers[i]->notifyWakeUp();
		}
	}
}
