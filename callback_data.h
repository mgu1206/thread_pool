#pragma once
class callback_data
{
public:
	callback_data() : _job_id(0) {};
	callback_data(unsigned long long job_id): _job_id(job_id) {};
	virtual ~callback_data() {};

	long long _job_id;
};

