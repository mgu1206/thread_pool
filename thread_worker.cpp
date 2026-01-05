#include "thread_worker.h"

thread_worker::thread_worker(job_priority job_priority)
{
	this->_terminated = false;
	this->_job_priority = job_priority;

	this->setJobMatchPriorities();
}

thread_worker::~thread_worker()
{
	this->stopWorker();

}

void thread_worker::setJobManager(std::shared_ptr<job_manager> job_manager)
{
	this->_job_manager = job_manager;
}

void thread_worker::startWorker()
{
	this->stopWorker();

	this->_terminated = false;

	this->_worker_thread = std::jthread(&thread_worker::worker_function, this);
}

void thread_worker::stopWorker()
{
	this->_terminated = true;

	if (this->_worker_thread.joinable())
	{
		this->_worker_condition.notify_one();
		this->_worker_thread.request_stop();
		this->_worker_thread.join();
	}
}

job_priority thread_worker::getPriority()
{
	return this->_job_priority;
}

void thread_worker::setJobMatchPriorities()
{
	switch (this->_job_priority)
	{
		case job_priority::HIGH_PRIORITY:
		{
			this->_job_match_priorities = { job_priority::HIGH_PRIORITY, job_priority::NORMAL_PRIORITY };
			break;
		}

		case job_priority::NORMAL_PRIORITY:
		{
			this->_job_match_priorities = { job_priority::NORMAL_PRIORITY, job_priority::LOW_PRIORITY, job_priority::HIGH_PRIORITY };
			break;
		}

		case job_priority::LOW_PRIORITY:
		{
			this->_job_match_priorities = { job_priority::LOW_PRIORITY, job_priority::NORMAL_PRIORITY, job_priority::HIGH_PRIORITY };
			break;
		}

		default:
			this->_job_match_priorities = { job_priority::HIGH_PRIORITY, job_priority::NORMAL_PRIORITY, job_priority::LOW_PRIORITY };
			break;
	}
}

void thread_worker::notifyWakeUp()
{
	this->_worker_condition.notify_one();
}

void thread_worker::jobCountChanged()
{
	this->_worker_condition.notify_all();
}

bool thread_worker::checkwakeUpCondition()
{
	if (this->_terminated == true)
	{
		return true;
	}

	std::shared_ptr<job_manager> manager = this->_job_manager.lock();

	if (manager == nullptr)
	{
		return false;
	}

	return manager->getJobCount(this->_job_match_priorities) > 0;
}

void thread_worker::worker_function()
{
	while (this->_terminated != true)
	{
		std::shared_ptr<job> cur_job = nullptr;

		std::unique_lock<std::mutex> locker(this->_worker_mutex);
		this->_worker_condition.wait(locker, [this] {return this->checkwakeUpCondition(); });

		std::shared_ptr<job_manager> manager = this->_job_manager.lock();

		if (manager == nullptr)
		{
			continue;
		}

		// get job that match thread's priority.
		// if there is no job match priority, thread find lower priority job than itself's priority(in priority range)
		cur_job = manager->pop_job(this->_job_match_priorities);

		if (!cur_job)
		{
			continue;
		}

		manager.reset();
		locker.unlock();

		if (this->_terminated) break;
		if (cur_job == nullptr || cur_job.use_count() == 0) continue;

		cur_job->setJobManager(this->_job_manager);
		cur_job->work();
	}
}