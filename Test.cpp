#include <iostream>
#include "Duration.h"
#include "src/AStar.h"

int main()
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
	param.allow_corner = false;
	param.start = AStar::Vec2(0, 0);
	param.end = AStar::Vec2(9, 9);
	param.is_canreach = [&](const AStar::Vec2 &pos)->bool
	{
		return maps[pos.y][pos.x] == 0;
	};

	// 执行搜索
	AStar as;
	Duration duration;
	auto path = as.search(param);

	std::cout << (path.empty() ? "路径未找到！" : "路径已找到！") << std::endl;
	std::cout << "本次寻路耗时" << duration.nano_seconds() << "纳秒" << std::endl;
	std::cout << '\n';
	system("pause");
	return 0;
}
