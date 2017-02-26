#include <chrono>
#include <iostream>
#include "a_star.h"
#include "blockallocator.h"

int main(int argc, char *argv[])
{
    // 生生成随机地图
    srand((unsigned)time(NULL));
    char maps[1000][1000];
    for (size_t i = 0; i < 1000; ++i)
    {
        for (size_t j = 0; j < 1000; ++j)
        {
            maps[i][j] = rand() % 3 == 0 ? 1 : 0;
        }
    }

	// 搜索参数
	AStar::Params param;
	param.width = 1000;
	param.height = 1000;
	param.corner = false;
	param.start = AStar::Vec2(0, 0);
	param.end = AStar::Vec2(999, 999);
	param.can_pass = [&](const AStar::Vec2 &pos)->bool
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
	std::cout << "本次寻路耗时" << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << "毫秒" << std::endl;

	return 0;
}
