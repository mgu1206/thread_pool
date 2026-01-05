#pragma once

#include <memory>
#include <vector>
#include <mutex>
#include <map>
#include <functional>

#include "job.h"

class job_manager : public std::enable_shared_from_this<job_manager>
{
public:
	job_manager();
	virtual ~job_manager();

public:
	std::shared_ptr<job_manager> getPtr();

public:
	void push_job(std::shared_ptr<job> new_job);
	std::shared_ptr<job> pop_job(std::vector<job_priority> job_priorities);

	int getAllJobCount();
	int getJobCount(std::vector<job_priority> job_priorities);
	void setWorkerNotification(const std::function<void(void)>& workerWakeUpNotification);

private:
	void workerWakeUpNotification();

private:
	std::mutex _job_mutex;
	std::map<job_priority, std::vector<std::shared_ptr<job>>> _priority_job_list;

	std::function<void(void)> _workerWakeUpNotification;
};

