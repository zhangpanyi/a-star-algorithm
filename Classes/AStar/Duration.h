#pragma once

#include <chrono>

class Duration
{
public:
	Duration()
		: start_time_(std::chrono::system_clock::now())
	{}

	/**
	 * 重设开始时间
	 */
	void Reset()
	{
		start_time_ = std::chrono::system_clock::now();
	}

	/**
	 * 返回计时开始到现在所经过的时间长度
	 * @return 单位秒
	 */
	std::chrono::seconds::rep Seconds()
	{
		return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start_time_).count();
	}

	/**
	 * 返回计时开始到现在所经过的时间长度
	 * @return 单位毫秒
	 */
	std::chrono::milliseconds::rep MilliSeconds()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start_time_).count();
	}

	/**
	 * 返回计时开始到现在所经过的时间长度
	 * @return 单位微秒
	 */
	std::chrono::microseconds::rep MicroSeconds()
	{
		return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - start_time_).count();
	}

	/**
	 * 返回计时开始到现在所经过的时间长度
	 * @return 单位纳秒
	 */
	std::chrono::nanoseconds::rep NanoSeconds()
	{
		return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now() - start_time_).count();
	}

private:
	std::chrono::system_clock::time_point start_time_;
};