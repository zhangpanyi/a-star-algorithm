#include <iostream>
#include <assert.h>
#include "AStar.h"

const int STEP = 10;
const int OBLIQUE = 14;

// 堆比较函数
bool HeapComp(const AStar::Node *a, const AStar::Node *b)
{
	return a->f() > b->f();
}

AStar::AStar()	: m_row(0)
	, m_col(0)
	, m_mapSize(0)
	, m_mapIndex(nullptr)
	, m_callBack(nullptr)
{
}

AStar::~AStar()
{
	if (m_mapIndex)
	{
		delete[] m_mapIndex;
	}
}

// 初始化
void AStar::init(const AStarDef &def)
{
	m_row = def.row;
	m_col = def.col;
	m_callBack = def.canReach;

	if (m_mapIndex)
	{
		if (m_mapSize < m_row * m_col)
		{
			delete[] m_mapIndex;
			m_mapSize = m_row * m_col;
			m_mapIndex = new NodeState[m_mapSize];
		}
	}
	else
	{
		m_mapSize = m_row * m_col;
		m_mapIndex = new NodeState[m_mapSize];
	}
}

// 清理
void AStar::clear()
{
	for (int index = 0; index < m_row * m_col; ++index)
	{
		if (m_mapIndex[index].ptr)
		{
			delete m_mapIndex[index].ptr;
			m_mapIndex[index].ptr = nullptr;
		}
		m_mapIndex[index].state = NOTEXIST;
	}

	m_row = 0;
	m_col = 0;
	m_openList.clear();
	m_callBack = nullptr;
}

// 格子是否存在于开启列表
inline AStar::Node* AStar::isExistInOpenList(const Grid &grid)
{
	NodeState &node = m_mapIndex[grid.row * m_row + grid.col];
	return node.state == IN_OPENLIST ? node.ptr : nullptr;
}

// 格子是否存在于关闭列表
inline bool AStar::isExistInCloseList(const Grid &point)
{
	return m_mapIndex[point.row * m_row + point.col].state == IN_CLOSELIST;
}

// 查询格子是否可通行
bool AStar::isCanReach(const Grid &target)
{
	if (target.col >= 0 && target.col < m_col && target.row >= 0 && target.row < m_row)
	{
		return m_callBack(target);
	}
	else
	{
		return false;
	}
}

// 查询格子是否可到达
bool AStar::isCanReached(const Grid &current, const Grid &target, bool allowCorner)
{
	if (!isCanReach(target) || isExistInCloseList(target))
	{
		return false;
	}

	if (abs(current.col - target.col) + abs(current.row - target.row) == 1)
	{
		return true;
	}
	else if (allowCorner)
	{
		return (isCanReach(Grid(current.col + target.col - current.col, current.row))
			&& isCanReach(Grid(current.col, current.row + target.row - current.row)));
	}

	return false;
}

// 搜索可通行的格子
void AStar::searchCanReached(std::vector<Grid> &around, const Grid &current, bool allowCorner)
{
	Grid target;
	for (int row = current.row - 1; row <= current.row + 1; ++row)
	{
		for (int col = current.col - 1; col <= current.col + 1; ++col)
		{
			target.col = col;
			target.row = row;
			if (isCanReached(current, target, allowCorner))
			{
				around.push_back(target);
			}
		}
	}
}

// 计算G值
inline int AStar::calculG(Node *parent, const Grid &current)
{
	int value = ((abs(current.col - parent->pos.col) + abs(current.row - parent->pos.row)) == 2 ? OBLIQUE : STEP);
	value += parent->g;
	return value;
}

// 计算H值
inline int AStar::calculH(const Grid &current, const Grid &end)
{
	int value = abs(end.col - current.col) + abs(end.row - current.row);
	return value * STEP;
}

// 获取节点在开启列表中的索引值
int AStar::getIndex(Node *pNode)
{
	for (unsigned int index = 0; index < m_openList.size(); ++index)
	{
		if (m_openList[index]->pos == pNode->pos)
		{
			return index;
		}
	}
	return -1;
}

// 开启列表上滤(二叉堆上滤)
void AStar::percolateUp(int hole)
{
	int parent = 0;
	while (hole > 1)
	{
		parent = (hole - 1) / 2;
		if (m_openList[hole]->f() < m_openList[parent]->f())
		{
			std::swap(m_openList[hole], m_openList[parent]);
			hole = parent;
		}
		else
		{
			break;
		}
	}
}

// 当节点存在于开启列表中的处理函数
void AStar::foundNode(Node *currentNode, Node *newNode)
{
	int newG = calculG(currentNode, newNode->pos);

	if (newG < newNode->g)
	{
		newNode->g = newG;
		newNode->parent = currentNode;

		percolateUp(getIndex(newNode));
	}
}

// 当节点不存在于开启列表中的处理函数
void AStar::notFoundNode(Node *currentNode, Node *newNode, const Grid &end)
{
	newNode->parent = currentNode;
	newNode->g = calculG(currentNode, newNode->pos);
	newNode->h = calculH(newNode->pos, end);

	NodeState &node = m_mapIndex[newNode->pos.row * m_row + newNode->pos.col];
	node.ptr = newNode;
	node.state = IN_OPENLIST;

	m_openList.push_back(newNode);
	std::push_heap(m_openList.begin(), m_openList.end(), HeapComp);
}

// A*算法参数定义是否有效
inline bool AStar::validAStarDef(const AStarDef &def)
{
	if (def.canReach
		&& (def.col >= 0 && def.row >= 0)
		&& (def.start.col >= 0 && def.start.col < def.col)
		&& (def.start.row >= 0 && def.start.row < def.row)
		&& (def.end.col >= 0 && def.end.col < def.col)
		&& (def.end.row >= 0 && def.end.row < def.row)
		)
	{
		return true;
	}
	return false;
}

// 执行A*搜索
std::deque<Grid> AStar::search(const AStarDef &def)
{
	std::deque<Grid> searchPath;
	if (validAStarDef(def))
	{
		// 初始化
		init(def);

		// 周围可通行格
		std::vector<Grid> around;
		around.reserve(def.allowCorner ? 8 : 4);

		// 将起点放入开启列表
		Node *start = new Node(def.start);
		m_openList.push_back(start);

		// 更新地图索引
		NodeState &node = m_mapIndex[start->pos.row * m_row + start->pos.col];
		node.ptr = start;
		node.state = IN_OPENLIST;

		while (!m_openList.empty())
		{
			// 取出F值最小的节点
			Node *currentNode = m_openList[0];
			std::pop_heap(m_openList.begin(), m_openList.end(), HeapComp);
			m_openList.pop_back();

			// 放入关闭列表
			m_mapIndex[currentNode->pos.row * m_row + currentNode->pos.col].state = IN_CLOSELIST;

			// 搜索可通行格子
			around.clear();
			searchCanReached(around, currentNode->pos, def.allowCorner);

			// 遍历可通行格子
			unsigned int size = around.size();
			for (unsigned int index = 0; index < size; ++index)
			{
				Node *newNode = isExistInOpenList(around[index]);
				if (newNode)
				{
					foundNode(currentNode, newNode);
				}
				else
				{
					newNode = new Node(around[index]);
					notFoundNode(currentNode, newNode, def.end);

					if (around[index] == def.end)
					{
						while (newNode->parent)
						{
							searchPath.push_front(newNode->pos);
							newNode = newNode->parent;
						}
						goto EndSearch;
					}
				}
			}
		}
EndSearch:
		clear();
	}
	else
	{
		std::cerr << "Invalid AStarDef!" << std::endl;
		assert(false);
	}

	return searchPath;
}
