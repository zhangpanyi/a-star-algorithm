#include "AStar.h"
#include <cassert>
#include <cstring>
#include <algorithm>
#include "BlockAllocator.h"

static const int kStepValue = 10;
static const int kObliqueValue = 14;

AStar::AStar(BlockAllocator *allocator)
	: width_(0)
	, height_(0)
    , allocator_(allocator)
	, step_value_(kStepValue)
	, oblique_value_(kObliqueValue)
{
    assert(allocator_ != nullptr);
}

AStar::~AStar()
{
	clear();
}

// 获取直行估值
int AStar::get_step_value() const
{
	return step_value_;
}

// 获取拐角估值
int AStar::get_oblique_value() const
{
	return oblique_value_;
}

// 设置直行估值
void AStar::set_step_value(int value)
{
	step_value_ = value;
}

// 获取拐角估值
void AStar::set_oblique_value(int value)
{
	oblique_value_ = value;
}

// 清理参数
void AStar::clear()
{
	size_t index = 0;
	const size_t max_size = width_ * height_;
	while (index < max_size)
	{
        allocator_->free(mapping_[index++], sizeof(Node));
        index++;
	}
	open_list_.clear();
    query_cb_ = nullptr;
	width_ = height_ = 0;
}

// 初始化参数
void AStar::init_param(const Param &param)
{
	width_ = param.width;
	height_ = param.height;
    query_cb_ = param.can_reach;
	if (!mapping_.empty())
	{
		memset(&mapping_[0], 0, sizeof(Node*) * mapping_.size());
	}
	mapping_.resize(width_ * height_, nullptr);
}

// 参数是否有效
bool AStar::is_vlid_param(const Param &param)
{
	return (param.can_reach != nullptr
			&& (param.width > 0 && param.height > 0)
			&& (param.end.x >= 0 && param.end.x < param.width)
			&& (param.end.y >= 0 && param.end.y < param.height)
			&& (param.start.x >= 0 && param.start.x < param.width)
			&& (param.start.y >= 0 && param.start.y < param.height)
			);
}

// 获取节点索引
bool AStar::get_node_index(Node *node, size_t &index)
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

// 二叉堆上滤
void AStar::percolate_up(size_t hole)
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

// 计算G值
inline uint16_t AStar::calcul_g_value(Node *parent_node, const Vec2 &current_pos)
{
	uint16_t g_value = ((abs(current_pos.y + current_pos.x - parent_node->pos.y - parent_node->pos.x)) == 2 ? oblique_value_ : step_value_);
	return g_value += parent_node->g;
}

// 计算F值
inline uint16_t AStar::calcul_h_value(const Vec2 &current_pos, const Vec2 &end_pos)
{
	unsigned int h_value = abs(end_pos.y + end_pos.x - current_pos.y - current_pos.x);
	return h_value * step_value_;
}

// 节点是否存在于开启列表
inline bool AStar::has_noode_in_open_list(const Vec2 &pos, Node *&out)
{
	out = mapping_[pos.y * width_ + pos.x];
	return out ? out->state == IN_OPENLIST : false;
}

// 节点是否存在于关闭列表
inline bool AStar::has_node_in_close_list(const Vec2 &pos)
{
	Node *node_ptr = mapping_[pos.y * width_ + pos.x];
	return node_ptr ? node_ptr->state == IN_CLOSELIST : false;
}

// 是否可到达
bool AStar::can_reach(const Vec2 &pos)
{
	return (pos.x >= 0 && pos.x < width_ && pos.y >= 0 && pos.y < height_) ? query_cb_(pos) : false;
}

// 当前点是否可到达目标点
bool AStar::can_reach(const Vec2 &current_pos, const Vec2 &target_pos, bool corner)
{
	if (target_pos.x >= 0 && target_pos.x < width_ && target_pos.y >= 0 && target_pos.y < height_)
	{
		if (has_node_in_close_list(target_pos))
		{
			return false;
		}

		if (abs(current_pos.y + current_pos.x - target_pos.y - target_pos.x) == 1)
		{
			return query_cb_(target_pos);
		}
		else if (corner)
		{
			return (can_reach(Vec2(current_pos.x + target_pos.x - current_pos.x, current_pos.y))
					&& can_reach(Vec2(current_pos.x, current_pos.y + target_pos.y - current_pos.y)));
		}
	}
	return false;
}

// 查找附近可到达的位置
void AStar::find_can_reach_pos(const Vec2 &current_pos, bool corner, std::vector<Vec2> &lists)
{
    lists.clear();
    Vec2 target_pos;
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
			if (can_reach(current_pos, target_pos, corner))
			{
                lists.push_back(target_pos);
			}
			++col_index;
		}
		++row_index;
	}
}

// 处理找到节点的情况
void AStar::handle_found_node(Node *current_node, Node *target_node)
{
	unsigned int g_value = calcul_g_value(current_node, target_node->pos);
	if (g_value < target_node->g)
	{
		target_node->g = g_value;
		target_node->parent = current_node;

		size_t index = 0;
		if (get_node_index(target_node, index))
		{
			percolate_up(index);
		}
		else
		{
			assert(false);
		}
	}
}

// 处理未找到节点的情况
void AStar::handle_not_found_node(Node *current_node, Node *target_node, const Vec2 &end_pos)
{
	target_node->parent = current_node;
	target_node->h = calcul_h_value(target_node->pos, end_pos);
	target_node->g = calcul_g_value(current_node, target_node->pos);

	Node *&node_ptr = mapping_[target_node->pos.y * width_ + target_node->pos.x];
	node_ptr = target_node;
	node_ptr->state = IN_OPENLIST;

	open_list_.push_back(target_node);
	std::push_heap(open_list_.begin(), open_list_.end(), [](const Node *a, const Node *b)->bool
	{
		return a->f() > b->f();
	});
}

// 执行寻路操作
std::vector<AStar::Vec2> AStar::find(const Param &param)
{
	std::vector<Vec2> paths;
	if (!is_vlid_param(param))
	{
		assert(false);
	}
	else
	{
		init_param(param);
		std::vector<Vec2> nearby_nodes;
		nearby_nodes.reserve(param.corner ? 8 : 4);

		Node *start_node = new(allocator_->allocate(sizeof(Node))) Node(param.start);
		open_list_.push_back(start_node);

		Node *&node_ptr = mapping_[start_node->pos.y * width_ + start_node->pos.x];
		node_ptr = start_node;
		node_ptr->state = IN_OPENLIST;

		while (!open_list_.empty())
		{
			Node *current_node = *open_list_.begin();
			std::pop_heap(open_list_.begin(), open_list_.end(), [](const Node *a, const Node *b)->bool
			{
				return a->f() > b->f();
			});
			open_list_.pop_back();
			mapping_[current_node->pos.y * width_ + current_node->pos.x]->state = IN_CLOSELIST;

			find_can_reach_pos(current_node->pos, param.corner, nearby_nodes);

			size_t index = 0;
			const size_t size = nearby_nodes.size();
			while (index < size)
			{
				Node *new_node = nullptr;
				if (has_noode_in_open_list(nearby_nodes[index], new_node))
				{
					handle_found_node(current_node, new_node);
				}
				else
				{
					new_node = new(allocator_->allocate(sizeof(Node))) Node(nearby_nodes[index]);
					handle_not_found_node(current_node, new_node, param.end);

					if (nearby_nodes[index] == param.end)
					{
						while (new_node->parent)
						{
							paths.push_back(new_node->pos);
							new_node = new_node->parent;
						}
						std::reverse(paths.begin(), paths.end());
						goto __end__;
					}
				}
				++index;
			}
		}
	}

__end__:
	clear();
	return paths;
}