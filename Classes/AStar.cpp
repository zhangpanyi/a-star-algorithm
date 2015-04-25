#include <assert.h>
#include <algorithm>
#include "AStar.h"

const int kStep = 10;
const int kOblique = 14;

inline bool CompHeap(const AStar::Node *a, const AStar::Node *b)
{
	return a->f() > b->f();
}

AStar::AStar()
	: num_row_(0)
	, num_col_(0)
	, num_map_size_(0)
	, query_func_(nullptr)
{
}

AStar::~AStar()
{

}

void AStar::Init(const AStarDef &def)
{
	num_row_ = def.row;
	num_col_ = def.col;
	query_func_ = def.can_reach;
	num_map_size_ = num_row_ * num_col_;

	if (!maps_index_.empty())
	{
		memset(&maps_index_[0], 0, sizeof(std::vector<Node *>::value_type) * maps_index_.size());
	}
	maps_index_.resize(num_map_size_, nullptr);
}

void AStar::Clear()
{
	unsigned int index = 0;
	while (index < num_map_size_)
	{
		if (maps_index_[index])
		{
			delete maps_index_[index];
			maps_index_[index] = nullptr;
		}
		++index;
	}

	num_row_ = 0;
	num_col_ = 0;
	num_map_size_ = 0;
	open_list_.resize(0);
	query_func_ = nullptr;
}

inline AStar::Node* AStar::IsExistInOpenList(const Point &point)
{
	Node *node_ptr = maps_index_[point.row * num_row_ + point.col];
	return node_ptr ? (node_ptr->state == IN_OPENLIST ? node_ptr : nullptr) : nullptr;
}

inline bool AStar::IsExistInCloseList(const Point &point)
{
	Node *node_ptr = maps_index_[point.row * num_row_ + point.col];
	return node_ptr ? node_ptr->state == IN_CLOSELIST : false;
}

bool AStar::IsCanReach(const Point &target_point)
{
	return (target_point.col >= 0 && target_point.col < num_col_ && target_point.row >= 0 && target_point.row < num_row_) ? query_func_(target_point) : false;
}

bool AStar::IsCanReachAndInOpen(const Point &target_point)
{
	if (target_point.col >= 0 && target_point.col < num_col_ && target_point.row >= 0 && target_point.row < num_row_)
	{
		return IsExistInCloseList(target_point) ? false : query_func_(target_point);
	}
	else
	{
		return false;
	}
}

bool AStar::IsCanReached(const Point &current_point, const Point &target_point, bool allow_corner)
{
	if (!IsCanReachAndInOpen(target_point))
	{
		return false;
	}

	if (abs(current_point.row + current_point.col - target_point.row - target_point.col) == 1)
	{
		return true;
	}
	else if (allow_corner)
	{
		return (IsCanReach(Point(current_point.col + target_point.col - current_point.col, current_point.row))
				&& IsCanReach(Point(current_point.col, current_point.row + target_point.row - current_point.row)));
	}

	return false;
}

void AStar::SearchCanReached(const Point &current_point, bool allow_corner, std::vector<Point> &surround_point)
{
	Point target;
	surround_point.clear();

	for (int row = current_point.row - 1; row <= current_point.row + 1; ++row)
	{
		for (int col = current_point.col - 1; col <= current_point.col + 1; ++col)
		{
			if (IsCanReached(current_point, target(row, col), allow_corner))
			{
				surround_point.push_back(target);
			}
		}
	}
}

inline unsigned int AStar::CalculG(Node *parent, const Point &current_point)
{
	unsigned int g_value = ((abs(current_point.row + current_point.col - parent->pos.row - parent->pos.col)) == 2 ? kOblique : kStep);
	return g_value += parent->g;
}

inline unsigned int AStar::CalculH(const Point &current_point, const Point &end_point)
{
	unsigned int h_value = abs(end_point.row + end_point.col - current_point.row - current_point.col);
	return h_value * kStep;
}

int AStar::GetIndex(Node *node)
{
	unsigned int index = 0;
	const unsigned int size = open_list_.size();

	while (index < size)
	{
		if (open_list_[index]->pos == node->pos)
		{
			return index;
		}
		++index;
	}

	return -1;
}

void AStar::PercolateUp(int hole)
{
	unsigned int parent = 0;
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
			return;
		}
	}
}

void AStar::FoundNode(Node *current_point, Node *new_point)
{
	unsigned int g_value = CalculG(current_point, new_point->pos);

	if (g_value < new_point->g)
	{
		new_point->g = g_value;
		new_point->parent = current_point;
		PercolateUp(GetIndex(new_point));
	}
}

void AStar::NotFoundNode(Node *current_point, Node *new_point, const Point &end_point)
{
	new_point->parent = current_point;
	new_point->g = CalculG(current_point, new_point->pos);
	new_point->h = CalculH(new_point->pos, end_point);

	Node *&node_ptr = maps_index_[new_point->pos.row * num_row_ + new_point->pos.col];
	assert(node_ptr == nullptr);
	node_ptr = new_point;
	node_ptr->state = IN_OPENLIST;

	open_list_.push_back(new_point);
	std::push_heap(open_list_.begin(), open_list_.end(), CompHeap);
}

inline bool AStar::ValidParameter(const AStarDef &def)
{
	return (def.can_reach
			&& (def.col >= 0 && def.row >= 0)
			&& (def.start_point.col >= 0 && def.start_point.col < def.col)
			&& (def.start_point.row >= 0 && def.start_point.row < def.row)
			&& (def.end_point.col >= 0 && def.end_point.col < def.col)
			&& (def.end_point.row >= 0 && def.end_point.row < def.row)
			);
}

std::deque<Point> AStar::Search(const AStarDef &def)
{
	std::deque<Point> search_path;
	if (ValidParameter(def))
	{
		Init(def);

		std::vector<Point> around_point;
		around_point.reserve(def.allow_corner ? 8 : 4);

		Node *start_node = new Node(def.start_point);
		open_list_.push_back(start_node);

		Node *&node_ptr = maps_index_[start_node->pos.row * num_row_ + start_node->pos.col];
		assert(node_ptr == nullptr);
		node_ptr = start_node;
		node_ptr->state = IN_OPENLIST;

		while (!open_list_.empty())
		{
			Node *current_point = *open_list_.begin();
			std::pop_heap(open_list_.begin(), open_list_.end(), CompHeap);
			open_list_.pop_back();
			maps_index_[current_point->pos.row * num_row_ + current_point->pos.col]->state = IN_CLOSELIST;

			SearchCanReached(current_point->pos, def.allow_corner, around_point);

			unsigned int index = 0;
			const unsigned int size = around_point.size();

			while (index < size)
			{
				Node *new_point = IsExistInOpenList(around_point[index]);
				if (new_point)
				{
					FoundNode(current_point, new_point);
				}
				else
				{
					new_point = new Node(around_point[index]);
					NotFoundNode(current_point, new_point, def.end_point);

					if (around_point[index] == def.end_point)
					{
						while (new_point->parent)
						{
							search_path.push_front(new_point->pos);
							new_point = new_point->parent;
						}
						goto end_search;
					}
				}
				++index;
			}
		}

	end_search:
		Clear();
	}
	else
	{
		Clear();
		assert("invalid parameter!");
	}

	return search_path;
}