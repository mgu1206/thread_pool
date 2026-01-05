#include "job.h"

job::job(unsigned long long job_id)
{
	_job_id = job_id;
	_job_data = nullptr;
	_job_priority = job_priority::NORMAL_PRIORITY;
	_job_callback_1 = nullptr;
	_job_callback_2 = nullptr;
}

job::job(unsigned long long job_id, const std::function<void(std::vector<unsigned char >)>& job_callback) :
	job(job_id)
{
	_job_callback_1 = job_callback;
}

job::job(unsigned long long job_id, job_priority job_priority, const std::function<void(std::vector<unsigned char >)>& job_callback) :
	job(job_id, job_callback)
{
	_job_priority = job_priority;
}

job::job(unsigned long long job_id, job_data* job_data, const std::function<void(std::vector<unsigned char >)>& job_callback) :
	job(job_id)
{
	_job_data = job_data;
	_job_callback_1 = job_callback;
}

job::job(unsigned long long job_id, job_priority job_priority, job_data* job_data, const std::function<void(std::vector<unsigned char >)>& job_callback) :
	job(job_id, job_data, job_callback)
{
	_job_priority = job_priority;
}

job::job(const std::function<void(std::vector<unsigned char >)>& job_callback) :
	job(0)
{
	_job_callback_1 = job_callback;
	_job_callback_2 = nullptr;
}

job::job(job_priority job_priority, const std::function<void(std::vector<unsigned char >)>& job_callback) :
	job(job_callback)
{
	_job_priority = job_priority;
}

job::job(unsigned long long job_id, const std::function<void(callback_data*)>& job_callback) :
	job(job_id)
{
	_job_id = job_id;
	_job_callback_2 = job_callback;
}

job::job(unsigned long long job_id, job_priority job_priority, const std::function<void(callback_data*)>& job_callback) :
	job(job_id, job_callback)
{
	_job_priority = job_priority;
}

job::job(unsigned long long job_id, job_data* job_data, const std::function<void(callback_data*)>& job_callback) :
	job(job_id)
{
	_job_id = job_id;
	_job_data = job_data;
	_job_callback_2 = job_callback;
}

job::job(unsigned long long job_id, job_priority job_priority, job_data* job_data, const std::function<void(callback_data*)>& job_callback) :
	job(job_id, job_data, job_callback)
{
	_job_priority = job_priority;
}

job::job(const std::function<void(callback_data*)>& job_callback) :
	job(0)
{
	_job_callback_1 = nullptr;
	_job_callback_2 = job_callback;
}

job::job(job_priority job_priority, const std::function<void(callback_data*)>& job_callback) :
	job(job_callback)
{
	_job_priority = job_priority;
}

job::~job()
{
	_job_callback_1 = nullptr;
	_job_callback_2 = nullptr;

	if (_job_data != nullptr)
	{
		delete _job_data;
		_job_data = nullptr;
	}
}

void job::setJobId(const unsigned long long job_id)
{
	this->_job_id = job_id;
}

void job::setJobData(job_data* job_data)
{
	_job_data = job_data;
}

void job::setJobPriority(job_priority job_priority)
{
	_job_priority = job_priority;
}

unsigned long long job::getJobId()
{
	return _job_id;
}

job_data* job::getJobData()
{
	return _job_data;
}

job_priority job::getJobPriority()
{
	return _job_priority;
}

void job::setJobManager(std::weak_ptr<job_manager> job_manager)
{
	_job_manager = job_manager;
}

std::shared_ptr<job> job::getPtr()
{
	return shared_from_this();
}