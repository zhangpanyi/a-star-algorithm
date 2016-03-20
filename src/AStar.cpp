#include "AStar.h"
#include <cassert>
#include <cstring>
#include <algorithm>

const int kStepValue = 10;
const int kObliqueValue = 14;

AStar::AStar()
	: width_(0)
	, height_(0)
	, step_value_(kStepValue)
	, oblique_value_(kObliqueValue)
{

}

AStar::~AStar()
{
	Clear();
}

int AStar::StepValue() const
{
	return step_value_;
}

int AStar::ObliqueValue() const
{
	return oblique_value_;
}

void AStar::SetStepValue(int value)
{
	step_value_ = value;
}

void AStar::SetObliqueValue(int value)
{
	oblique_value_ = value;
}

void AStar::Clear()
{
	size_t index = 0;
	const size_t max_size = width_ * height_;
	while (index < max_size)
	{
		delete maps_[index++];
	}
	query_ = nullptr;
	open_list_.clear();
	width_ = height_ = 0;
}

void AStar::InitParam(const Param &param)
{
	width_ = param.width;
	height_ = param.height;
	query_ = param.is_canreach;
	if (!maps_.empty())
	{
		memset(&maps_[0], 0, sizeof(Node *) * maps_.size());
	}
	maps_.resize(width_ * height_, nullptr);
}

bool AStar::IsVlidParam(const Param &param)
{
	return (param.is_canreach
			&& (param.width > 0 && param.height > 0)
			&& (param.end.x >= 0 && param.end.x < param.width)
			&& (param.end.y >= 0 && param.end.y < param.height)
			&& (param.start.x >= 0 && param.start.x < param.width)
			&& (param.start.y >= 0 && param.start.y < param.height)
			);
}

bool AStar::GetNodeIndex(Node *node, size_t &index)
{
	index = 0;
	const size_t size = open_list_.size();
	while (index < size)
	{
		if (open_list_[index]->pos == node->pos)
		{
			return true;
		}
		++index;
	}
	return false;
}

void AStar::PercolateUp(size_t hole)
{
	size_t parent = 0;
	while (hole > 0)
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

inline uint16_t AStar::CalculGValue(Node *parent_node, const Vec2 &current_pos)
{
	uint16_t g_value = ((abs(current_pos.y + current_pos.x - parent_node->pos.y - parent_node->pos.x)) == 2 ? oblique_value_ : step_value_);
	return g_value += parent_node->g;
}

inline uint16_t AStar::CalculHValue(const Vec2 &current_pos, const Vec2 &end_pos)
{
	unsigned int h_value = abs(end_pos.y + end_pos.x - current_pos.y - current_pos.x);
	return h_value * step_value_;
}

inline bool AStar::HasNoodeInOpenList(const Vec2 &pos, Node *&out)
{
	out = maps_[pos.y * height_ + pos.x];
	return out ? out->state == IN_OPENLIST : false;
}

inline bool AStar::HasNodeInCloseList(const Vec2 &pos)
{
	Node *node_ptr = maps_[pos.y * height_ + pos.x];
	return node_ptr ? node_ptr->state == IN_CLOSELIST : false;
}

bool AStar::Canreach(const Vec2 &pos)
{
	return (pos.x >= 0 && pos.x < width_ && pos.y >= 0 && pos.y < height_) ? query_(pos) : false;
}

bool AStar::Canreach(const Vec2 &current_pos, const Vec2 &target_pos, bool allow_corner)
{
	if (target_pos.x >= 0 && target_pos.x < width_ && target_pos.y >= 0 && target_pos.y < height_)
	{
		if (HasNodeInCloseList(target_pos))
		{
			return false;
		}

		if (abs(current_pos.y + current_pos.x - target_pos.y - target_pos.x) == 1)
		{
			return query_(target_pos);
		}
		else if (allow_corner)
		{
			return (Canreach(Vec2(current_pos.x + target_pos.x - current_pos.x, current_pos.y))
					&& Canreach(Vec2(current_pos.x, current_pos.y + target_pos.y - current_pos.y)));
		}
	}
	return false;
}

void AStar::FindCanreachPos(const Vec2 &current_pos, bool allow_corner, std::vector<Vec2> &canreach_pos)
{
	Vec2 target_pos;
	canreach_pos.clear();
	int row_index = current_pos.y - 1;
	const int max_row = current_pos.y + 1;
	const int max_col = current_pos.x + 1;

	if (row_index < 0)
	{
		row_index = 0;
	}
	
	while (row_index <= max_row)
	{
		int col_index = current_pos.x - 1;

		if (col_index < 0)
		{
			col_index = 0;
		}
	
		while (col_index <= max_col)
		{
			target_pos.set(col_index, row_index);
			if (Canreach(current_pos, target_pos, allow_corner))
			{
				canreach_pos.push_back(target_pos);
			}
			++col_index;
		}
		++row_index;
	}
}

void AStar::HandleFoundNode(Node *current_node, Node *target_node)
{
	unsigned int g_value = CalculGValue(current_node, target_node->pos);
	if (g_value < target_node->g)
	{
		target_node->g = g_value;
		target_node->parent = current_node;

		size_t index = 0;
		if (GetNodeIndex(target_node, index))
		{
			PercolateUp(index);
		}
		else
		{
			assert(false);
		}
	}
}

void AStar::HndleNotFoundNode(Node *current_node, Node *target_node, const Vec2 &end_pos)
{
	target_node->parent = current_node;
	target_node->h = CalculHValue(target_node->pos, end_pos);
	target_node->g = CalculGValue(current_node, target_node->pos);

	Node *&node_ptr = maps_[target_node->pos.y * height_ + target_node->pos.x];
	node_ptr = target_node;
	node_ptr->state = IN_OPENLIST;

	open_list_.push_back(target_node);
	std::push_heap(open_list_.begin(), open_list_.end(), [](const Node *a, const Node *b)->bool
	{
		return a->f() > b->f();
	});
}

std::deque<AStar::Vec2> AStar::Search(const Param &param)
{
	std::deque<Vec2> paths;
	if (!IsVlidParam(param))
	{
		assert(false);
	}
	else
	{
		InitParam(param);
		std::vector<Vec2> nearby_nodes;
		nearby_nodes.reserve(param.allow_corner ? 8 : 4);

		// 起点放入开启列表
		Node *start_node = new Node(param.start);
		open_list_.push_back(start_node);

		// 设置起点所对应节点的状态
		Node *&node_ptr = maps_[start_node->pos.y * height_ + start_node->pos.x];
		node_ptr = start_node;
		node_ptr->state = IN_OPENLIST;

		while (!open_list_.empty())
		{
			// 取出F值最小的节点
			Node *current_node = *open_list_.begin();
			std::pop_heap(open_list_.begin(), open_list_.end(), [](const Node *a, const Node *b)->bool
			{
				return a->f() > b->f();
			});
			open_list_.pop_back();
			maps_[current_node->pos.y * height_ + current_node->pos.x]->state = IN_CLOSELIST;

			// 搜索附近可通行的位置
			FindCanreachPos(current_node->pos, param.allow_corner, nearby_nodes);

			size_t index = 0;
			const size_t size = nearby_nodes.size();
			while (index < size)
			{
				// 如果存在于开启列表
				Node *new_node = nullptr;
				if (HasNoodeInOpenList(nearby_nodes[index], new_node))
				{
					HandleFoundNode(current_node, new_node);
				}
				else
				{
					// 如果不存在于开启列表
					new_node = new Node(nearby_nodes[index]);
					HndleNotFoundNode(current_node, new_node, param.end);

					// 找到终点
					if (nearby_nodes[index] == param.end)
					{
						while (new_node->parent)
						{
							paths.push_front(new_node->pos);
							new_node = new_node->parent;
						}
						goto __end__;
					}
				}
				++index;
			}
		}
	}

__end__:
	Clear();
	return paths;
}