#include <time.h>
#include <vector>
#include <iostream>
#include "AStar.h"
using namespace std;

int main()
{
	char aStarMap[1000][1000] = {0};

	auto CanReach = [&](const Point &point)
	{
		return aStarMap[point.y][point.x] == 0;
	};

	AStarDef def;
	def.row = 1000;
	def.col = 1000;
	def.start = Point(0, 0);
	def.end = Point(999, 999);
	def.allowCorner = true;
	def.canReach = CanReach;

	clock_t start, end;
	start = clock();

	AStar astar;
	std::deque<Point> path = astar(def);

	end = clock();
	cout << "Run time: " << (double)(end - start) / CLOCKS_PER_SEC << "s" << endl;

	system("pause");

	return 0;
}