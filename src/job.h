#pragma once

#include <memory>
#include <vector>
#include <functional>

enum job_priority
{
	HIGH_PRIORITY,
	NORMAL_PRIORITY,
	LOW_PRIORITY,
};

class thread_worker;
class job_manager;
class job : public std::enable_shared_from_this<job>
{
public:
	job(unsigned long long job_id);

	// Lambda-based constructors
	job(unsigned long long job_id, std::function<void()> work_func);
	job(unsigned long long job_id, job_priority job_priority, std::function<void()> work_func);
	job(std::function<void()> work_func);
	job(job_priority job_priority, std::function<void()> work_func);

	virtual ~job();

	unsigned long long _job_id;
	std::weak_ptr<thread_worker> _worker;

public:
	void setJobId(const unsigned long long job_id);
	void setJobPriority(job_priority job_priority);

public:
	unsigned long long getJobId();
	job_priority getJobPriority();

public:
	void setJobManager(std::weak_ptr<job_manager> job_manager);

public:
	std::shared_ptr<job> getPtr();

public:
	virtual void work();

private:
	job_priority _job_priority;

	std::weak_ptr<job_manager> _job_manager;

	// Lambda work function storage
	std::function<void()> _work_function;
};

