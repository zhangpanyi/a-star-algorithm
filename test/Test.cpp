#include <chrono>
#include <iostream>
#include "AStar.h"

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

int main(int argc, char *argv[])
{
	char maps[1000][1000] =
	{
		{ 0, 1, 0, 0, 0, 1, 0, 0, 0, 0 },
		{ 0, 0, 0, 1, 0, 1, 0, 1, 0, 1 },
		{ 1, 1, 1, 1, 0, 1, 0, 1, 0, 1 },
		{ 0, 0, 0, 1, 0, 0, 0, 1, 0, 1 },
		{ 0, 1, 0, 1, 1, 1, 1, 1, 0, 1 },
		{ 0, 1, 0, 0, 0, 0, 0, 0, 0, 1 },
		{ 0, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 0, 0, 0, 0, 1, 0, 0, 0, 1, 0 },
		{ 1, 1, 0, 0, 1, 0, 1, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 1, 0, 1, 0 },
	};

	// 搜索参数
	AStar::Param param;
	param.width = 1000;
	param.height = 1000;
	param.corner = false;
	param.start = AStar::Vec2(0, 0);
	param.end = AStar::Vec2(999, 999);
	param.can_reach = [&](const AStar::Vec2 &pos)->bool
	{
		return maps[pos.y][pos.x] == 0;
	};

	// 执行搜索
	AStar as;
	Duration duration;
	auto path = as.find(param);
	std::cout << (path.empty() ? "路径未找到！" : "路径已找到！") << std::endl;
	std::cout << "本次寻路耗时" << duration.nano_seconds() << "纳秒" << std::endl;
	std::cout << '\n';

	return 0;
}