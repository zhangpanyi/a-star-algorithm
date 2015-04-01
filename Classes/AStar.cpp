#include <assert.h>
#include <iostream>
#include <algorithm>
#include "AStar.h"

const int STEP = 10;
const int OBLIQUE = 14;

// 堆比较函数
bool HeapComp(const AStar::Node *a, const AStar::Node *b)
{
	return a->f() > b->f();
}

AStar::AStar()	: nRow_(0)
	, nCol_(0)
	, nMapSize_(0)
	, pMapIndex_(nullptr)
	, callBack_(nullptr)
{
}

AStar::~AStar()
{
	if (pMapIndex_)
	{
		delete[] pMapIndex_;
	}
}

// 初始化
void AStar::init(const AStarDef &def)
{
	nRow_ = def.row;
	nCol_ = def.col;
	callBack_ = def.canReach;

	if (pMapIndex_)
	{
		if (nMapSize_ < nRow_ * nCol_)
		{
			delete[] pMapIndex_;
			nMapSize_ = nRow_ * nCol_;
			pMapIndex_ = new NodeState[nMapSize_];
		}
	}
	else
	{
		nMapSize_ = nRow_ * nCol_;
		pMapIndex_ = new NodeState[nMapSize_];
	}
}

// 清理
void AStar::clear()
{
	for (int index = 0; index < nRow_ * nCol_; ++index)
	{
		if (pMapIndex_[index].ptr)
		{
			delete pMapIndex_[index].ptr;
			pMapIndex_[index].ptr = nullptr;
		}
		pMapIndex_[index].state = NOTEXIST;
	}

	nRow_ = 0;
	nCol_ = 0;
	openList_.clear();
	callBack_ = nullptr;
}

// 格子是否存在于开启列表
inline AStar::Node* AStar::isExistInOpenList(const Grid &grid)
{
	NodeState &node = pMapIndex_[grid.row * nRow_ + grid.col];
	return node.state == IN_OPENLIST ? node.ptr : nullptr;
}

// 格子是否存在于关闭列表
inline bool AStar::isExistInCloseList(const Grid &point)
{
	return pMapIndex_[point.row * nRow_ + point.col].state == IN_CLOSELIST;
}

// 查询格子是否可通行
bool AStar::isCanReach(const Grid &target)
{
	if (target.col >= 0 && target.col < nCol_ && target.row >= 0 && target.row < nRow_)
	{
		return callBack_(target);
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
	for (unsigned int index = 0; index < openList_.size(); ++index)
	{
		if (openList_[index]->pos == pNode->pos)
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
		if (openList_[hole]->f() < openList_[parent]->f())
		{
			std::swap(openList_[hole], openList_[parent]);
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

	NodeState &node = pMapIndex_[newNode->pos.row * nRow_ + newNode->pos.col];
	node.ptr = newNode;
	node.state = IN_OPENLIST;

	openList_.push_back(newNode);
	std::push_heap(openList_.begin(), openList_.end(), HeapComp);
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
		openList_.push_back(start);

		// 更新地图索引
		NodeState &node = pMapIndex_[start->pos.row * nRow_ + start->pos.col];
		node.ptr = start;
		node.state = IN_OPENLIST;

		while (!openList_.empty())
		{
			// 取出F值最小的节点
			Node *currentNode = openList_[0];
			std::pop_heap(openList_.begin(), openList_.end(), HeapComp);
			openList_.pop_back();

			// 放入关闭列表
			pMapIndex_[currentNode->pos.row * nRow_ + currentNode->pos.col].state = IN_CLOSELIST;

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
						goto _EndSearch_;
					}
				}
			}
		}
_EndSearch_:
		clear();
	}
	else
	{
		std::cerr << "Invalid AStarDef!" << std::endl;
		assert(false);
	}

	return searchPath;
}
