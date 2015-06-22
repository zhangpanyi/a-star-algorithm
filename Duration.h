#pragma once

#include <chrono>

class Duration
{
public:
	Duration()
		: start_time_(std::chrono::system_clock::now())
	{}

	void Reset()
	{
		start_time_ = std::chrono::system_clock::now();
	}

	std::chrono::seconds::rep Seconds()
	{
		return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start_time_).count();
	}

	std::chrono::milliseconds::rep MilliSeconds()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start_time_).count();
	}

	std::chrono::microseconds::rep MicroSeconds()
	{
		return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - start_time_).count();
	}

	std::chrono::nanoseconds::rep NanoSeconds()
	{
		return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now() - start_time_).count();
	}

private:
	std::chrono::system_clock::time_point start_time_;
};