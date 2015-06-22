#include "AStar.h"

#include <cassert>
#include <algorithm>
#include "../Misc/BlockAllocator.h"

namespace a_star
{
	/************************************************************************/

	/* Node state */
	enum
	{
		NOTEXIST,
		IN_OPENLIST,
		IN_CLOSELIST
	};

	const int kStep = 10;
	const int kOblique = 14;

	struct Node
	{
		unsigned short	g;
		unsigned short	h;
		Vec2			pos;
		int				state;
		Node*			parent;

		int f() const { return g + h; }

		inline Node(const Vec2 &pos) : g(0), h(0), pos(pos), parent(nullptr), state(NOTEXIST) {}

		void* operator new(std::size_t size)
		{
			return SOA::GetInstance()->Allocate(size);
		}

			void operator delete(void* p) throw()
		{
			if (p) SOA::GetInstance()->Free(p, sizeof(Node));
		}
	};

	inline bool CompHeap(const Node *a, const Node *b)
	{
#ifdef _DEBUG
		assert(a && b);
#endif
		return a->f() > b->f();
	}

	/************************************************************************/

	AStar::AStar()
		: map_size_(0)
		, total_row_(0)
		, total_col_(0)
		, query_func_(nullptr)
	{
	}

	AStar::~AStar()
	{

	}

	void AStar::Init(const AStarParam &param)
	{
		total_row_ = param.total_row;
		total_col_ = param.total_col;
		query_func_ = param.is_can_reach;
		map_size_ = total_row_ * total_col_;

		if (!maps_index_.empty())
		{
			memset(&maps_index_[0], 0, sizeof(Node *) * maps_index_.size());
		}
		maps_index_.resize(map_size_, nullptr);
	}

	void AStar::Clear()
	{
		unsigned int index = 0;
		while (index < map_size_)
		{
			delete maps_index_[index++];
		}

		map_size_ = 0;
		total_row_ = 0;
		total_col_ = 0;
		open_list_.resize(0);
		query_func_ = nullptr;
	}

	bool AStar::InvalidParam(const AStarParam &param)
	{
		return (!param.is_can_reach
				|| (param.total_col < 0 || param.total_row < 0)
				|| (param.start_point.col < 0 || param.start_point.col >= param.total_col)
				|| (param.start_point.row < 0 || param.start_point.row >= param.total_row)
				|| (param.end_point.col < 0 || param.end_point.col >= param.total_col)
				|| (param.end_point.row < 0 || param.end_point.row >= param.total_row)
				);
	}

	inline bool AStar::HasNodeInOpenList(const Vec2 &point, Node *&out)
	{
		out = maps_index_[point.row * total_row_ + point.col];
		return out ? out->state == IN_OPENLIST : false;
	}

	inline bool AStar::HasNodeInCloseList(const Vec2 &point)
	{
		Node *node_ptr = maps_index_[point.row * total_row_ + point.col];
		return node_ptr ? node_ptr->state == IN_CLOSELIST : false;
	}

	bool AStar::IsCanReach(const Vec2 &point)
	{
		return (point.col >= 0 && point.col < total_col_ && point.row >= 0 && point.row < total_row_) ? query_func_(point) : false;
	}

	bool AStar::IsCanReach(const Vec2 &current, const Vec2 &target, bool allow_corner)
	{
		if (target.col >= 0 && target.col < total_col_ && target.row >= 0 && target.row < total_row_)
		{
			if (HasNodeInCloseList(target)) return false;
			if (abs(current.row + current.col - target.row - target.col) == 1)
			{
				return query_func_(target);
			}
			else if (allow_corner)
			{
				return (IsCanReach(Vec2(current.col + target.col - current.col, current.row))
						&& IsCanReach(Vec2(current.col, current.row + target.row - current.row)));
			}
		}	

		return false;
	}

	void AStar::SearchCanReachThePosition(const Vec2 &current, bool allow_corner, std::vector<Vec2> &can_each_pos)
	{
		Vec2 target;
		can_each_pos.clear();

		int row_index = current.row - 1;
		const int max_row = current.row + 1;
		const int max_col = current.col + 1;

		while (row_index <= max_row)
		{
			int col_index = current.col - 1;
			while (col_index <= max_col)
			{
				if (IsCanReach(current, target(row_index, col_index), allow_corner))
				{
					can_each_pos.push_back(target);
				}
				++col_index;
			}
			++row_index;
		}
	}

	inline unsigned int AStar::CalculG(Node *parent, const Vec2 &current)
	{
#ifdef _DEBUG
		assert(parent);
#endif
		unsigned int g_value = ((abs(current.row + current.col - parent->pos.row - parent->pos.col)) == 2 ? kOblique : kStep);
		return g_value += parent->g;
	}

	inline unsigned int AStar::CalculH(const Vec2 &current, const Vec2 &end_point)
	{
		unsigned int h_value = abs(end_point.row + end_point.col - current.row - current.col);
		return h_value * kStep;
	}

	int AStar::GetNodeIndex(Node *node)
	{
#ifdef _DEBUG
		assert(node);
#endif
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
		std::abort();
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

	void AStar::HandleFoundNode(Node *current, Node *target)
	{
#ifdef _DEBUG
		assert(current && target);
#endif
		unsigned int g_value = CalculG(current, target->pos);
		if (g_value < target->g)
		{
			target->g = g_value;
			target->parent = current;
			PercolateUp(GetNodeIndex(target));
		}
	}

	void AStar::HandleNotFoundNode(Node *current, Node *target, const Vec2 &end_point)
	{
#ifdef _DEBUG
		assert(current && target);
#endif
		target->parent = current;
		target->g = CalculG(current, target->pos);
		target->h = CalculH(target->pos, end_point);

		Node *&node_ptr = maps_index_[target->pos.row * total_row_ + target->pos.col];
		node_ptr = target;
		node_ptr->state = IN_OPENLIST;

		open_list_.push_back(target);
		std::push_heap(open_list_.begin(), open_list_.end(), CompHeap);
	}

	std::vector<Vec2> AStar::Search(const AStarParam &param)
	{
		std::vector<Vec2> search_path;
		if (!InvalidParam(param))
		{
			Init(param);

			std::vector<Vec2> around_node_pos;
			around_node_pos.reserve(param.allow_corner ? 8 : 4);

			Node *start_node = new Node(param.start_point);
			open_list_.push_back(start_node);

			Node *&node_ptr = maps_index_[start_node->pos.row * total_row_ + start_node->pos.col];
			node_ptr = start_node;
			node_ptr->state = IN_OPENLIST;

			while (!open_list_.empty())
			{
				Node *current_node = *open_list_.begin();
				std::pop_heap(open_list_.begin(), open_list_.end(), CompHeap);
				open_list_.pop_back();
				maps_index_[current_node->pos.row * total_row_ + current_node->pos.col]->state = IN_CLOSELIST;

				SearchCanReachThePosition(current_node->pos, param.allow_corner, around_node_pos);

				unsigned int index = 0;
				const unsigned int size = around_node_pos.size();

				while (index < size)
				{
					Node *new_node = nullptr;
					if (HasNodeInOpenList(around_node_pos[index], new_node))
					{
						HandleFoundNode(current_node, new_node);
					}
					else
					{
						new_node = new Node(around_node_pos[index]);
						HandleNotFoundNode(current_node, new_node, param.end_point);

						if (around_node_pos[index] == param.end_point)
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
}