#include "thread_worker.h"

thread_worker::thread_worker(job_priority job_priority)
{
	_terminated = false;
	_job_priority = job_priority;
	_worker_thread = nullptr;

	setJobMatchPriorities();
}

thread_worker::~thread_worker()
{
	stopWorker();
	
}

void thread_worker::setJobManager(std::shared_ptr<job_manager> job_manager)
{
	_job_manager = job_manager;
}

void thread_worker::startWorker()
{
	stopWorker();

	_terminated = false;

	_worker_thread = new std::thread(&thread_worker::worker_function, this);
}

void thread_worker::stopWorker()
{
	_terminated = true;

	if (_worker_thread != nullptr && _worker_thread->joinable() == true)
	{
		_worker_condition.notify_one();
		_worker_thread->join();
	}
}

job_priority thread_worker::getPriority()
{
	return _job_priority;
}

void thread_worker::setJobMatchPriorities()
{
	switch (_job_priority)
	{
		case job_priority::HIGH_PRIORITY:
		{
			_job_match_priorities = { job_priority::HIGH_PRIORITY, job_priority::NORMAL_PRIORITY };
			break;
		}

		case job_priority::NORMAL_PRIORITY:
		{
			_job_match_priorities = { job_priority::NORMAL_PRIORITY, job_priority::LOW_PRIORITY, job_priority::HIGH_PRIORITY };
			break;
		}

		case job_priority::LOW_PRIORITY:
		{
			_job_match_priorities = { job_priority::LOW_PRIORITY, job_priority::NORMAL_PRIORITY, job_priority::HIGH_PRIORITY };
			break;
		}

		default:
			_job_match_priorities = { job_priority::HIGH_PRIORITY, job_priority::NORMAL_PRIORITY, job_priority::LOW_PRIORITY };
			break;
	}
}

void thread_worker::notifyWakeUp()
{
	_worker_condition.notify_one();
}

void thread_worker::jobCountChanged()
{
	_worker_condition.notify_all();
}

bool thread_worker::checkwakeUpCondition()
{
	if (_terminated == true)
	{
		return true;
	}

	std::shared_ptr<job_manager> manager = _job_manager.lock();

	if (manager == nullptr)
	{
		return false;
	}

	return manager->getJobCount(_job_match_priorities) > 0;
}

void thread_worker::worker_function()
{
	while (_terminated != true)
	{
		std::shared_ptr<job> cur_job = nullptr;

		std::unique_lock<std::mutex> locker(_worker_mutex);
		_worker_condition.wait(locker, [this] {return checkwakeUpCondition(); });

		std::shared_ptr<job_manager> manager = _job_manager.lock();

		if (manager == nullptr)
		{
			continue;
		}

		// get job that match thread's priority.
		// if there is no job match priority, thread find lower priority job than itself's priority(in priority range)
		cur_job = manager->pop_job(_job_match_priorities);

		if (!cur_job)
		{
			continue;
		}

		manager.reset();
		locker.unlock();

		if (_terminated) break;
		if (cur_job == nullptr || cur_job.use_count() == 0) continue;

		cur_job->setJobManager(_job_manager);
		cur_job->work();
	}
}