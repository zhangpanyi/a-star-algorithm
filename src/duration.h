/**
 * 时间长度计算
 */

#pragma once

#include <chrono>

class Duration
{
public:
	Duration()
		: start_time_(std::chrono::system_clock::now())
	{}

	void reset()
	{
		start_time_ = std::chrono::system_clock::now();
	}

	std::chrono::seconds::rep seconds()
	{
		return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start_time_).count();
	}

	std::chrono::milliseconds::rep milli_seconds()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start_time_).count();
	}

	std::chrono::microseconds::rep micro_seconds()
	{
		return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - start_time_).count();
	}

	std::chrono::nanoseconds::rep nano_seconds()
	{
		return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now() - start_time_).count();
	}

private:
	std::chrono::system_clock::time_point start_time_;
};