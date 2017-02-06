#include <chrono>
#include <iostream>
#include "src/a-star.h"
#include "src/blockallocator.h"

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

	auto start_time = std::chrono::system_clock::now();
	auto path = algorithm.find(param);
	auto end_time = std::chrono::system_clock::now();

	std::cout << (path.empty() ? "路径未找到！" : "路径已找到！") << std::endl;
	std::cout << "本次寻路耗时" << std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count() << "纳秒" << std::endl;

	return 0;
}
