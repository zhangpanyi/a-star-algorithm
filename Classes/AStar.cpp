#include <assert.h>
#include <iostream>
#include "AStar.h"

const int STEP = 10;
const int OBLIQUE = 14;

bool HeapComp(const AStar::Node *a, const AStar::Node *b)
{
	return (a->g + a->h) > (b->g + b->h);
}

AStar::AStar()
	: m_row(0)
	, m_col(0)
	, m_canReach(nullptr)
{
}

AStar::~AStar()
{
}

void AStar::init(const AStarDef &def)
{
	m_row = def.row;
	m_col = def.col;
	m_canReach = def.canReach;

	m_allNodes = new NodeState*[m_row];
	for (int i = 0; i < m_row; ++i)
	{
		m_allNodes[i] = new NodeState[m_col];
	}

	assert(m_row > 0);
	assert(m_col > 0);
	assert(m_canReach);
	assert(m_openList.empty());
}

void AStar::clear()
{
	for (int i = 0; i < m_row; ++i)
	{
		for (int j = 0; j < m_col; ++j)
		{
			if (m_allNodes[i][j].ptr) delete m_allNodes[i][j].ptr;
		}

		delete[] m_allNodes[i];
	}
	delete[] m_allNodes;
	m_allNodes = nullptr;

	m_row = 0;
	m_col = 0;
	m_canReach = nullptr;
}

AStar::Node* AStar::isExistInOpenList(const Point &point)
{
	if (m_allNodes[point.y][point.x].state == INOPENLIST)
	{
		return m_allNodes[point.y][point.x].ptr;
	}
	return nullptr;
}

bool AStar::isExistInCloseList(const Point &point)
{
	return m_allNodes[point.y][point.x].state == INCLOSELIST;
}

bool AStar::isCanReach(const Point &target)
{
	if (target.x > -1 && target.x < m_col && target.y > -1 && target.y < m_row)
	{
		return m_canReach(target);
	}
	else
	{
		return false;
	}
}

bool AStar::isCanReach(const Point &current, const Point &target, bool allowCorner)
{
	if (!isCanReach(target) || isExistInCloseList(target))
	{
		return false;
	}

	if (abs(current.x - target.x) + abs(current.y - target.y) == 1)
	{
		return true;
	}
	else if (allowCorner)
	{
		int gapX = target.x - current.x;
		int gapY = target.y - current.y;
		return (isCanReach(Point(current.x + gapX, current.y))
			&& isCanReach(Point(current.x, current.y + gapY)));
	}

	return false;
}

void AStar::searchCanReach(std::vector<Point> &surround, const Point &current, bool allowCorner)
{
	Point target;
	for (int row = current.y - 1; row <= current.y + 1; ++row)
	{
		for (int col = current.x - 1; col <= current.x + 1; ++col)
		{
			target.x = col;
			target.y = row;
			if (isCanReach(current, target, allowCorner))
			{
				surround.push_back(target);
			}
		}
	}
}

int AStar::calculG(Node *lastNode, const Point &current)
{
	int value = ((abs(current.x - lastNode->pos.x) + abs(current.y - lastNode->pos.y)) == 2 ? OBLIQUE : STEP);
	value += lastNode->g;
	return value;
}

int AStar::calculH(const Point &current, const Point &end)
{
	int value = abs(end.x - current.x) + abs(end.y - current.y);
	return value * STEP;
}

void AStar::foundNode(Node *currentNode, Node *newNode)
{
	int newG = calculG(currentNode, newNode->pos);

	if (newG < newNode->g)
	{
		newNode->g = newG;
		newNode->last = currentNode;

		std::sort_heap(m_openList.begin(), m_openList.end(), HeapComp);
	}
}

void AStar::notFoundNode(Node *currentNode, Node *newNode, const Point &end)
{
	newNode->last = currentNode;
	newNode->g = calculG(currentNode, newNode->pos);
	newNode->h = calculH(newNode->pos, end);

	m_openList.push_back(newNode);

	m_allNodes[newNode->pos.y][newNode->pos.x].ptr = newNode;
	m_allNodes[newNode->pos.y][newNode->pos.x].state = INOPENLIST;

	std::push_heap(m_openList.begin(), m_openList.end(), HeapComp);
}

std::deque<Point> AStar::operator() (const AStarDef &def)
{
	init(def);

	std::deque<Point> searchPath;
	std::vector<Point> surround;
	surround.reserve(8);

	Node *start = new Node(def.start);
	m_openList.push_back(start);

	m_allNodes[start->pos.y][start->pos.x].ptr = start;
	m_allNodes[start->pos.y][start->pos.x].state = INOPENLIST;

	while (!m_openList.empty())
	{
		Node *currentNode = m_openList[0];
		std::pop_heap(m_openList.begin(), m_openList.end(), HeapComp);
		m_openList.pop_back();
		m_allNodes[currentNode->pos.y][currentNode->pos.x].state = INCLOSELIST;

		surround.clear();
		searchCanReach(surround, currentNode->pos, def.allowCorner);

		for (auto itr = surround.begin(); itr != surround.end(); ++itr)
		{
			Node *newNode = isExistInOpenList(*itr);
			if (newNode)
			{
				foundNode(currentNode, newNode);
			}
			else
			{
				newNode = new Node(*itr);
				notFoundNode(currentNode, newNode, def.end);

				if ((*itr) == def.end)
				{			
					while (newNode->last)
					{
						searchPath.push_front(newNode->pos);
						newNode = newNode->last;
					}
					m_openList.clear();
					break;
				}
			}
		}
	}

	clear();

	return searchPath;
}