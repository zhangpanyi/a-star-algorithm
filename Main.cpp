#include <time.h>
#include "AStar.h"
#include <iostream>
using namespace std;

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
	cout << "Run time: " << (double)(end - start) / CLOCKS_PER_SEC << "s" << endl;

	system("pause");
	return 0;
}
