#include <time.h>
#include "AStar.h"
#include <fstream>
#include <sstream>
#include <iostream>

int main()
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

	AStarDef def;
	def.row = 1000;
	def.col = 1000;
	def.can_reach = [&](const Point &grid)->bool
	{
		return maps[grid.row][grid.col] == 0;
	};
	def.start_point = Point(0, 0);
	def.end_point = Point(999, 999);
	def.allow_corner = false;

	clock_t start, end;
	start = clock();

	AStar astar;
	auto path = astar.Search(def);
	
	end = clock();
	std::cout << "Run time: " << (double)(end - start) / CLOCKS_PER_SEC << "s" << std::endl;
	std::cout << (path.empty() ? "路径未找到！" : "路径已找到！") << std::endl;

	if (!path.empty())
	{

		std::ofstream file;
		std::stringstream ss;
		file.open("search_path.txt");
		for (auto &value : path)
		{
			ss.str("");
			ss << "row:" << value.row << ", " << "col:" << value.col << "\n";
			file.write(ss.str().c_str(), ss.str().length());
		}
		file.close();
		std::cout << "路径已成功保存到search_path.txt文件!" << std::endl;
	}

	system("pause");
	return 0;
}
