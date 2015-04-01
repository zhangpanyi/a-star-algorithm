#include <time.h>
#include <vector>
#include <iostream>
#include "AStar.h"
using namespace std;

int main()
{
	char aStarMap[1000][1000] =
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

	auto CanReach = [&](const Grid &grid)
	{
		return aStarMap[grid.row][grid.col] == 0;
	};

	AStarDef def;
	def.row = 1000;
	def.col = 1000;
	def.start = Grid(0, 0);
	def.end = Grid(999, 999);
	def.allowCorner = false;
	def.canReach = CanReach;

	clock_t start, end;
	start = clock();

	AStar astar;
	std::deque<Grid> path = astar.search(def);

	end = clock();
	cout << "Run time: " << (double)(end - start) / CLOCKS_PER_SEC << "s" << endl;

	return 0;
}
