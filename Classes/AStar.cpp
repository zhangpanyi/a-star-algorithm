#include <assert.h>
#include <iostream>
#include <algorithm>
#include "AStar.h"

const int kStep = 10;
const int kOblique = 14;

bool HeapComp(const AStar::Node *a, const AStar::Node *b)
{
	return a->f() > b->f();
}

AStar::AStar()
: num_row_(0)
, num_col_(0)
, num_map_size_(0)
, map_index_(nullptr)
, callback_(nullptr)
{
}

AStar::~AStar()
{
	if (map_index_)
	{
		delete[] map_index_;
	}
}

void AStar::Init(const AStarDef &def)
{
	num_row_ = def.row;
	num_col_ = def.col;
	callback_ = def.reach;

	if (map_index_)
	{
		if (num_map_size_ < num_row_ * num_col_)
		{
			delete[] map_index_;
			num_map_size_ = num_row_ * num_col_;
			map_index_ = new NodeState[num_map_size_];
		}
	}
	else
	{
		num_map_size_ = num_row_ * num_col_;
		map_index_ = new NodeState[num_map_size_];
	}
}

void AStar::Clear()
{
	for (int index = 0; index < num_row_ * num_col_; ++index)
	{
		if (map_index_[index].ptr)
		{
			delete map_index_[index].ptr;
			map_index_[index].ptr = nullptr;
		}
		map_index_[index].state = NOTEXIST;
	}

	num_row_ = 0;
	num_col_ = 0;
	open_list_.clear();
	callback_ = nullptr;
}

inline AStar::Node* AStar::IsExistInOpenList(const Grid &grid)
{
	NodeState &node = map_index_[grid.row * num_row_ + grid.col];
	return node.state == IN_OPENLIST ? node.ptr : nullptr;
}

inline bool AStar::IsExistInCloseList(const Grid &grid)
{
	return map_index_[grid.row * num_row_ + grid.col].state == IN_CLOSELIST;
}

bool AStar::IsCanReach(const Grid &target)
{
	if (target.col >= 0 && target.col < num_col_ && target.row >= 0 && target.row < num_row_)
	{
		return callback_(target);
	}
	else
	{
		return false;
	}
}

bool AStar::IsCanReached(const Grid &current, const Grid &target, bool allow_corner)
{
	if (!IsCanReach(target) || IsExistInCloseList(target))
	{
		return false;
	}

	if (abs(current.col - target.col) + abs(current.row - target.row) == 1)
	{
		return true;
	}
	else if (allow_corner)
	{
		return (IsCanReach(Grid(current.col + target.col - current.col, current.row))
			&& IsCanReach(Grid(current.col, current.row + target.row - current.row)));
	}

	return false;
}

void AStar::SearchCanReached(std::vector<Grid> &around, const Grid &current, bool allow_corner)
{
	Grid target;
	for (int row = current.row - 1; row <= current.row + 1; ++row)
	{
		for (int col = current.col - 1; col <= current.col + 1; ++col)
		{
			target.col = col;
			target.row = row;
			if (IsCanReached(current, target, allow_corner))
			{
				around.push_back(target);
			}
		}
	}
}

inline int AStar::CalculG(Node *parent, const Grid &current)
{
	int value = ((abs(current.col - parent->pos.col) + abs(current.row - parent->pos.row)) == 2 ? kOblique : kStep);
	value += parent->g;
	return value;
}

inline int AStar::CalculH(const Grid &current, const Grid &end)
{
	int value = abs(end.col - current.col) + abs(end.row - current.row);
	return value * kStep;
}

int AStar::GetIndex(Node *node)
{
	for (unsigned int index = 0; index < open_list_.size(); ++index)
	{
		if (open_list_[index]->pos == node->pos)
		{
			return index;
		}
	}
	return -1;
}

void AStar::PercolateUp(int hole)
{
	int parent = 0;
	while (hole > 1)
	{
		parent = (hole - 1) / 2;
		if (open_list_[hole]->f() < open_list_[parent]->f())
		{
			std::swap(open_list_[hole], open_list_[parent]);
			hole = parent;
		}
		else
		{
			break;
		}
	}
}

void AStar::FoundNode(Node *current_grid, Node *new_grid)
{
	int new_G_value = CalculG(current_grid, new_grid->pos);

	if (new_G_value < new_grid->g)
	{
		new_grid->g = new_G_value;
		new_grid->parent = current_grid;
		PercolateUp(GetIndex(new_grid));
	}
}

void AStar::NotFoundNode(Node *current_grid, Node *new_grid, const Grid &end)
{
	new_grid->parent = current_grid;
	new_grid->g = CalculG(current_grid, new_grid->pos);
	new_grid->h = CalculH(new_grid->pos, end);

	NodeState &node = map_index_[new_grid->pos.row * num_row_ + new_grid->pos.col];
	node.ptr = new_grid;
	node.state = IN_OPENLIST;

	open_list_.push_back(new_grid);
	std::push_heap(open_list_.begin(), open_list_.end(), HeapComp);
}

inline bool AStar::ValidAStarDef(const AStarDef &def)
{
	return (def.reach
		&& (def.col >= 0 && def.row >= 0)
		&& (def.start.col >= 0 && def.start.col < def.col)
		&& (def.start.row >= 0 && def.start.row < def.row)
		&& (def.end.col >= 0 && def.end.col < def.col)
		&& (def.end.row >= 0 && def.end.row < def.row)
		);
}

std::deque<Grid> AStar::Search(const AStarDef &def)
{
	std::deque<Grid> search_path;
	if (ValidAStarDef(def))
	{
		// 初始化
		Init(def);

		// 周围可通行格
		std::vector<Grid> around;
		around.reserve(def.allow_corner ? 8 : 4);

		// 将起点放入开启列表
		Node *start = new Node(def.start);
		open_list_.push_back(start);

		// 更新地图索引
		NodeState &node = map_index_[start->pos.row * num_row_ + start->pos.col];
		node.ptr = start;
		node.state = IN_OPENLIST;

		while (!open_list_.empty())
		{
			// 取出F值最小的节点
			Node *current_grid = open_list_[0];
			std::pop_heap(open_list_.begin(), open_list_.end(), HeapComp);
			open_list_.pop_back();

			// 放入关闭列表
			map_index_[current_grid->pos.row * num_row_ + current_grid->pos.col].state = IN_CLOSELIST;

			// 搜索可通行格子
			around.clear();
			SearchCanReached(around, current_grid->pos, def.allow_corner);

			// 遍历可通行格子
			unsigned int size = around.size();
			for (unsigned int index = 0; index < size; ++index)
			{
				Node *new_grid = IsExistInOpenList(around[index]);
				if (new_grid)
				{
					FoundNode(current_grid, new_grid);
				}
				else
				{
					new_grid = new Node(around[index]);
					NotFoundNode(current_grid, new_grid, def.end);

					if (around[index] == def.end)
					{
						while (new_grid->parent)
						{
							search_path.push_front(new_grid->pos);
							new_grid = new_grid->parent;
						}
						goto EndSearch;
					}
				}
			}
		}
	EndSearch:
		Clear();
	}
	else
	{
		Clear();
		throw std::exception("Invalid AStarDef!");
	}

	return search_path;
}