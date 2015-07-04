#include "AStar.h"
#include <algorithm>

const int STEP_VALUE = 10;
const int OBLIQUE_VALUE = 14;

AStar::AStar()
	: map_size_(0)
	, map_height_(0)
	, map_width_(0)
	, query_func_(nullptr)
{

}

AStar::~AStar()
{

}

void AStar::Clear()
{
	unsigned int index = 0;
	while (index < map_size_)
	{
		delete maps_index_[index++];
	}

	map_size_ = 0;
	map_height_ = 0;
	map_width_ = 0;
	open_list_.resize(0);
	query_func_ = nullptr;
}

void AStar::Init(const Param &param)
{
	map_height_ = param.height;
	map_width_ = param.width;
	query_func_ = param.is_canreach;
	map_size_ = map_height_ * map_width_;

	if (!maps_index_.empty())
	{
		memset(&maps_index_[0], 0, sizeof(Node *) * maps_index_.size());
	}
	maps_index_.resize(map_size_, nullptr);
}

bool AStar::VlidParam(const Param &param)
{
	return (param.is_canreach
			&& (param.width > 0 && param.height > 0)
			&& (param.start.col >= 0 && param.start.col < param.width)
			&& (param.start.row >= 0 && param.start.row < param.height)
			&& (param.end.col >= 0 && param.end.col < param.width)
			&& (param.end.row >= 0 && param.end.row < param.height)
			);
}

bool AStar::GetIndexByNode(Node *node, size_t &index)
{
	index = 0;
	const size_t size = open_list_.size();

	while (index < size)
	{
		if (open_list_[index]->pos == node->pos)
		{
			return index > 0;
		}
		++index;
	}
	return index > 0;
}

void AStar::PercolateUp(size_t hole)
{
	size_t parent = 0;
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

inline unsigned int AStar::CalculG(Node *parent, const Location &current)
{
	unsigned int g_value = ((abs(current.row + current.col - parent->pos.row - parent->pos.col)) == 2 ? OBLIQUE_VALUE : STEP_VALUE);
	return g_value += parent->g;
}

inline unsigned int AStar::CalculH(const Location &current, const Location &end_point)
{
	unsigned int h_value = abs(end_point.row + end_point.col - current.row - current.col);
	return h_value * STEP_VALUE;
}

inline bool AStar::HasNodeInOpenList(const Location &point, Node *&out)
{
	out = maps_index_[point.row * map_height_ + point.col];
	return out ? out->state == IN_OPENLIST : false;
}

inline bool AStar::HasNodeInCloseList(const Location &point)
{
	Node *node_ptr = maps_index_[point.row * map_height_ + point.col];
	return node_ptr ? node_ptr->state == IN_CLOSELIST : false;
}

bool AStar::IsCanReach(const Location &point)
{
	return (point.col >= 0 && point.col < map_width_ && point.row >= 0 && point.row < map_height_) ? query_func_(point) : false;
}

bool AStar::IsCanReach(const Location &current, const Location &target, bool allow_corner)
{
	if (target.col >= 0 && target.col < map_width_ && target.row >= 0 && target.row < map_height_)
	{
		if (HasNodeInCloseList(target)) return false;
		if (abs(current.row + current.col - target.row - target.col) == 1)
		{
			return query_func_(target);
		}
		else if (allow_corner)
		{
			return (IsCanReach(Location(current.col + target.col - current.col, current.row))
					&& IsCanReach(Location(current.col, current.row + target.row - current.row)));
		}
	}

	return false;
}

void AStar::SearchCanReach(const Location &current, bool allow_corner, std::vector<Location> &out)
{
	out.clear();
	Location target;

	int row_index = current.row - 1;
	const int max_row = current.row + 1;
	const int max_col = current.col + 1;

	while (row_index <= max_row)
	{
		int col_index = current.col - 1;
		while (col_index <= max_col)
		{
			if (IsCanReach(current, target.Reset(row_index, col_index), allow_corner))
			{
				out.push_back(target);
			}
			++col_index;
		}
		++row_index;
	}
}

void AStar::HandleFoundNode(Node *current, Node *target)
{
	unsigned int g_value = CalculG(current, target->pos);
	if (g_value < target->g)
	{
		target->g = g_value;
		target->parent = current;

		size_t index = 0;
		if (GetIndexByNode(target, index))
		{
			PercolateUp(index);
		}
	}
}

void AStar::HandleNotFoundNode(Node *current, Node *target, const Location &end_point)
{
	target->parent = current;
	target->g = CalculG(current, target->pos);
	target->h = CalculH(target->pos, end_point);

	Node *&node_ptr = maps_index_[target->pos.row * map_height_ + target->pos.col];
	node_ptr = target;
	node_ptr->state = IN_OPENLIST;

	open_list_.push_back(target);
	std::push_heap(open_list_.begin(), open_list_.end(), [](const Node *a, const Node *b)->bool
	{
		return a->f() > b->f();
	});
}

std::vector<AStar::Location> AStar::Search(const Param &param)
{
	std::vector<Location> search_path;
	if (VlidParam(param))
	{
		Init(param);

		// 存放附近的节点位置
		std::vector<Location> nearby_node;
		nearby_node.reserve(param.corner ? 8 : 4);

		// 起点放入开启列表
		Node *start_node = new Node(param.start);
		open_list_.push_back(start_node);

		// 设置起点所对应节点的状态
		Node *&node_ptr = maps_index_[start_node->pos.row * map_height_ + start_node->pos.col];
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
			maps_index_[current_node->pos.row * map_height_ + current_node->pos.col]->state = IN_CLOSELIST;

			// 搜索附近可通行的位置
			SearchCanReach(current_node->pos, param.corner, nearby_node);

			size_t index = 0;
			const size_t size = nearby_node.size();
			while (index < size)
			{
				// 如果存在于开启列表
				Node *new_node = nullptr;
				if (HasNodeInOpenList(nearby_node[index], new_node))
				{
					HandleFoundNode(current_node, new_node);
				}
				else
				{
					// 如果不存在于开启列表
					new_node = new Node(nearby_node[index]);
					HandleNotFoundNode(current_node, new_node, param.end);

					// 如果找到
					if (nearby_node[index] == param.end)
					{
						while (new_node->parent)
						{
							search_path.push_back(new_node->pos);
							new_node = new_node->parent;
						}
						std::reverse(search_path.begin(), search_path.end());
						goto __END__;
					}
				}
				++index;
			}
		}
	__END__:
		Clear();
	}
	else
	{
		Clear();
		throw std::runtime_error("invalid param!");
	}

	return search_path;
}