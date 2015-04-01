#include <time.h>
#include <iostream>
#include "AStar.h"
using namespace std;

int main()
{
	char astar_map[1000][1000] =
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

	auto CanReach = [&](const Grid &grid)->bool
	{
		return astar_map[grid.row][grid.col] == 0;
	};

	AStarDef def;
	def.row = 1000;
	def.col = 1000;
	def.reach = CanReach;
	def.start = Grid(0, 0);
	def.end = Grid(999, 999);
	def.allow_corner = false;

	clock_t start, end;
	start = clock();

	try
	{
		AStar astar;
		astar.Search(def);
	}
	catch (std::exception e)
	{
		cout << e.what() << endl;
	}
	
	end = clock();
	cout << "Run time: " << (double)(end - start) / CLOCKS_PER_SEC << "s" << endl;

	system("pause");
	return 0;
}
