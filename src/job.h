#pragma once

#include <memory>
#include <vector>
#include <functional>

#include "job_data.h"
#include "callback_data.h"

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
	job(unsigned long long job_id, const std::function<void(std::vector<unsigned char >)>& job_callback);
	job(unsigned long long job_id, job_priority job_priority, const std::function<void(std::vector<unsigned char >)>& job_callback);
	job(unsigned long long job_id, std::unique_ptr<job_data>&& job_data, const std::function<void(std::vector<unsigned char >)>& job_callback);
	job(unsigned long long job_id, job_priority job_priority, std::unique_ptr<job_data>&& job_data, const std::function<void(std::vector<unsigned char >)>& job_callback);
	job(const std::function<void(std::vector<unsigned char >)>& job_callback);
	job(job_priority job_priority, const std::function<void(std::vector<unsigned char >)>& job_callback);

	job(unsigned long long job_id, const std::function<void(std::shared_ptr<callback_data>)>& job_callback);
	job(unsigned long long job_id, job_priority job_priority, const std::function<void(std::shared_ptr<callback_data>)>& job_callback);
	job(unsigned long long job_id, std::unique_ptr<job_data>&& job_data, const std::function<void(std::shared_ptr<callback_data>)>& job_callback);
	job(unsigned long long job_id, job_priority job_priority, std::unique_ptr<job_data>&& job_data, const std::function<void(std::shared_ptr<callback_data>)>& job_callback);
	job(const std::function<void(std::shared_ptr<callback_data>)>& job_callback);
	job(job_priority job_priority, const std::function<void(std::shared_ptr<callback_data>)>& job_callback);

	// Lambda-based constructors
	job(unsigned long long job_id, std::function<void()> work_func);
	job(unsigned long long job_id, job_priority job_priority, std::function<void()> work_func);
	job(unsigned long long job_id, std::unique_ptr<job_data>&& job_data, std::function<void()> work_func);
	job(unsigned long long job_id, job_priority job_priority, std::unique_ptr<job_data>&& job_data, std::function<void()> work_func);
	job(std::function<void()> work_func);
	job(job_priority job_priority, std::function<void()> work_func);

	// Lambda + callback_1 (binary) constructors
	job(unsigned long long job_id, std::function<void()> work_func, const std::function<void(std::vector<unsigned char>)>& job_callback);
	job(unsigned long long job_id, job_priority job_priority, std::function<void()> work_func, const std::function<void(std::vector<unsigned char>)>& job_callback);
	job(unsigned long long job_id, std::unique_ptr<job_data>&& job_data, std::function<void()> work_func, const std::function<void(std::vector<unsigned char>)>& job_callback);
	job(unsigned long long job_id, job_priority job_priority, std::unique_ptr<job_data>&& job_data, std::function<void()> work_func, const std::function<void(std::vector<unsigned char>)>& job_callback);

	// Lambda + callback_2 (structured) constructors
	job(unsigned long long job_id, std::function<void()> work_func, const std::function<void(std::shared_ptr<callback_data>)>& job_callback);
	job(unsigned long long job_id, job_priority job_priority, std::function<void()> work_func, const std::function<void(std::shared_ptr<callback_data>)>& job_callback);
	job(unsigned long long job_id, std::unique_ptr<job_data>&& job_data, std::function<void()> work_func, const std::function<void(std::shared_ptr<callback_data>)>& job_callback);
	job(unsigned long long job_id, job_priority job_priority, std::unique_ptr<job_data>&& job_data, std::function<void()> work_func, const std::function<void(std::shared_ptr<callback_data>)>& job_callback);

	virtual ~job();

	unsigned long long _job_id;
	std::weak_ptr<thread_worker> _worker;

public:
	void setJobId(const unsigned long long job_id);
	void setJobData(std::unique_ptr<job_data>&& job_data);
	void setJobPriority(job_priority job_priority);

public:
	unsigned long long getJobId();
	job_data* getJobData();
	job_priority getJobPriority();

public:
	void setJobManager(std::weak_ptr<job_manager> job_manager);

public:
	std::shared_ptr<job> getPtr();

public:
	virtual void work();

private:
	std::unique_ptr<job_data> _job_data;
	job_priority _job_priority;

	std::weak_ptr<job_manager> _job_manager;

	// Lambda work function storage
	std::function<void()> _work_function;

public:
	std::function<void(std::vector<unsigned char >)> _job_callback_1;
	std::function<void(std::shared_ptr<callback_data>)> _job_callback_2;
};

