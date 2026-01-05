#include "job_manager.h"

job_manager::job_manager()
{
	_workerWakeUpNotification = nullptr;
}

job_manager::~job_manager()
{
}

void job_manager::push_job(std::shared_ptr<job> new_job)
{
	std::lock_guard<std::mutex> locker(_job_mutex);

	new_job->setJobManager(this->getPtr());

	auto iter = _priority_job_list.find(new_job->getJobPriority());

	if (iter == _priority_job_list.end())
	{
		_priority_job_list.insert({ new_job->getJobPriority(), { new_job } });
	}
	else
	{
		iter->second.push_back(new_job);
	}

	workerWakeUpNotification();
}

std::shared_ptr<job> job_manager::pop_job(std::vector<job_priority> job_priorities)
{
	std::lock_guard<std::mutex> locker(_job_mutex);

	if ((int)job_priorities.size() <= 0)
	{
		return nullptr;
	}

	for (int i = 0; i < (int)job_priorities.size(); i++)
	{
		auto iter = _priority_job_list.find(job_priorities[i]);

		if (iter != _priority_job_list.end())
		{
			if ((int)iter->second.size() <= 0)
			{
				continue;
			}

			std::shared_ptr<job> job = *(iter->second.begin());
			iter->second.erase(iter->second.begin());

			return job;
		}
	}

	return nullptr;
}

int job_manager::getAllJobCount()
{
	std::lock_guard<std::mutex> locker(_job_mutex);

	int count = 0;

	for (auto iter = _priority_job_list.begin(); iter != _priority_job_list.end(); iter++)
	{
		count += (int)iter->second.size();
	}

	return count;
}

int job_manager::getJobCount(std::vector<job_priority> job_priorities)
{
	std::lock_guard<std::mutex> locker(_job_mutex);

	if ((int)_priority_job_list.size() <= 0)
	{
		return 0;
	}

	for (int i = 0; i < (int)job_priorities.size(); i++)
	{
		auto iter = _priority_job_list.find(job_priorities[i]);

		if (iter == _priority_job_list.end()) continue;
		return (int)iter->second.size();
	}

	return 0;
}

void job_manager::setWorkerNotification(const std::function<void(void)>& workerWakeUpNotification)
{
	_workerWakeUpNotification = workerWakeUpNotification;
}

void job_manager::workerWakeUpNotification()
{
	if (_workerWakeUpNotification != nullptr)
	{
		_workerWakeUpNotification();
	}
}

std::shared_ptr<job_manager> job_manager::getPtr()
{
	return shared_from_this();
}