#include "job_manager.h"

job_manager::job_manager()
{
	this->_workerWakeUpNotification = nullptr;
}

job_manager::~job_manager()
{
}

void job_manager::push_job(std::shared_ptr<job> new_job)
{
	std::lock_guard<std::mutex> locker(this->_job_mutex);

	new_job->setJobManager(this->getPtr());

	auto iter = this->_priority_job_list.find(new_job->getJobPriority());

	if (iter == this->_priority_job_list.end())
	{
		this->_priority_job_list.insert({ new_job->getJobPriority(), { new_job } });
	}
	else
	{
		iter->second.push_back(new_job);
	}

	this->workerWakeUpNotification();
}

std::shared_ptr<job> job_manager::pop_job(std::vector<job_priority> job_priorities)
{
	std::lock_guard<std::mutex> locker(this->_job_mutex);

	if ((int)job_priorities.size() <= 0)
	{
		return nullptr;
	}

	for (int i = 0; i < (int)job_priorities.size(); i++)
	{
		auto iter = this->_priority_job_list.find(job_priorities[i]);

		if (iter != this->_priority_job_list.end())
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
	std::lock_guard<std::mutex> locker(this->_job_mutex);

	int count = 0;

	for (auto iter = this->_priority_job_list.begin(); iter != this->_priority_job_list.end(); iter++)
	{
		count += (int)iter->second.size();
	}

	return count;
}

int job_manager::getJobCount(std::vector<job_priority> job_priorities)
{
	std::lock_guard<std::mutex> locker(this->_job_mutex);

	if ((int)this->_priority_job_list.size() <= 0)
	{
		return 0;
	}

	int total_count = 0;

	for (int i = 0; i < (int)job_priorities.size(); i++)
	{
		auto iter = this->_priority_job_list.find(job_priorities[i]);

		if (iter != this->_priority_job_list.end())
		{
			total_count += (int)iter->second.size();
		}
	}

	return total_count;
}

void job_manager::setWorkerNotification(const std::function<void(void)>& workerWakeUpNotification)
{
	this->_workerWakeUpNotification = workerWakeUpNotification;
}

void job_manager::workerWakeUpNotification()
{
	if (this->_workerWakeUpNotification != nullptr)
	{
		this->_workerWakeUpNotification();
	}
}

std::shared_ptr<job_manager> job_manager::getPtr()
{
	return this->shared_from_this();
}