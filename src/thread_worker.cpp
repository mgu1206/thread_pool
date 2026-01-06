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

	// Use lambda to properly capture this and pass stop_token
	this->_worker_thread = std::jthread([this](std::stop_token st) {
		this->worker_function(st);
	});
}

void thread_worker::stopWorker()
{
	if (this->_worker_thread.joinable())
	{
		// Request stop first (sets stop_token)
		this->_worker_thread.request_stop();

		// Wake up the thread if it's waiting on condition variable
		this->_worker_condition.notify_all();

		// Wait for thread to finish
		this->_worker_thread.join();
	}

	// Set flag for compatibility (though stop_token is primary mechanism now)
	this->_terminated = true;
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

void thread_worker::worker_function(std::stop_token stop_token)
{
	while (!stop_token.stop_requested())
	{
		std::shared_ptr<job> cur_job = nullptr;

		std::unique_lock<std::mutex> locker(this->_worker_mutex);
		this->_worker_condition.wait(locker, stop_token, [this] {return this->checkwakeUpCondition(); });

		// Check if stop was requested during wait
		if (stop_token.stop_requested())
		{
			break;
		}

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

		if (stop_token.stop_requested()) break;
		if (cur_job == nullptr || cur_job.use_count() == 0) continue;

		cur_job->setJobManager(this->_job_manager);
		cur_job->work();
	}
}