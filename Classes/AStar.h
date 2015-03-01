/****************************************************************************
Copyright (c) 2015 Zhangpanyi

Created by Zhangpanyi on 2015

zhangpanyi@live.com
****************************************************************************/

#ifndef ASTAR_H
#define ASTAR_H

#include <deque>
#include <vector>
#include <functional>
#include "BlockAllocator.h"

#define NOTEXIST 0
#define INOPENLIST 1
#define INCLOSELIST 2

struct Point
{
	int			x;
	int			y;

	Point() : x(0), y(0) {}
	Point(int a, int b) : x(a), y(b) {}

	bool operator== (const Point &Incoming) const
	{
		return x == Incoming.x && y == Incoming.y;
	}
};

typedef std::function<bool(const Point&)> CanReach;

struct AStarDef
{
	bool		allowCorner;
	int			row;
	int			col;
	Point		start;
	Point		end;
	CanReach	canReach;

	AStarDef() : row(0), col(0), canReach(nullptr), allowCorner(false) {}
};

class AStar
{
public:
	struct Node
	{
		char	t;
		int		g;
		int		h;
		Point	pos;
		Node*	last;

		Node(const Point &p) : g(0), h(0), pos(p), last(nullptr) {}

		void* operator new(std::size_t size)
		{
			void *ptr = BlockAllocator::getInstance()->allocate(size);
			return ptr;
		}

			void operator delete(void* p) throw()
		{
			if (p) BlockAllocator::getInstance()->free(p, sizeof(Node));
		}
	};

	struct NodeState
	{
		char	state;
		Node*	ptr;

		NodeState() : ptr(nullptr), state(NOTEXIST) {};
	};

public:
	AStar();
	~AStar();

	std::deque<Point> operator() (const AStarDef &def);

private:
	void clear();

	void init(const AStarDef &def);

	Node* isExistInOpenList(const Point &point);
	bool isExistInCloseList(const Point &point);

	bool isCanReach(const Point &target);
	bool isCanReach(const Point &current, const Point &target, bool allowCorner);
	void searchCanReach(std::vector<Point> &surround, const Point &current, bool allowCorner);

	int calculG(Node *lastNode, const Point &current);
	int calculH(const Point &current, const Point &end);

	void foundNode(Node *currentNode, Node *newNode);
	void notFoundNode(Node *currentNode, Node *newNode, const Point &end);

private:
	int					m_row;
	int					m_col;
	CanReach			m_canReach;
	std::vector<Node*>	m_openList;
	NodeState*			m_allNodes;
};

#endif