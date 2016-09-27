#include <chrono>
#include <iostream>
#include "AStar.h"
#include "BlockAllocator.h"

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
	char maps[10][10] =
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
	param.width = 10;
	param.height = 10;
	param.corner = false;
	param.start = AStar::Vec2(0, 0);
	param.end = AStar::Vec2(9, 9);
	param.can_reach = [&](const AStar::Vec2 &pos)->bool
	{
		return maps[pos.y][pos.x] == 0;
	};

	// 执行搜索
    BlockAllocator allocator;
	AStar algorithm(&allocator);
	Duration duration;
	auto path = algorithm.find(param);
	std::cout << (path.empty() ? "路径未找到！" : "路径已找到！") << std::endl;
	std::cout << "本次寻路耗时" << duration.nano_seconds() << "纳秒" << std::endl;
	std::cout << '\n';

	return 0;
}