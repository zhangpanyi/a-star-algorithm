#include "AStar.h"
#include <algorithm>
#include "../Misc/BlockAllocator.h"

namespace pf
{
	/**
	 * 节点状态枚举
	 * 存在于开启列表或者关闭列表
	 */
	enum
	{
		NOTEXIST,
		IN_OPENLIST,
		IN_CLOSELIST
	};

	/**
	 * 路径估值
	 */
	const int kStep = 10;
	const int kOblique = 14;

	/**
	 * 路径节点
	 */
	struct Node
	{
		unsigned short	g;
		unsigned short	h;
		Point			pos;
		int				state;
		Node*			parent;

		int f() const { return g + h; }

		inline Node(const Point &pos) : g(0), h(0), pos(pos), parent(nullptr), state(NOTEXIST) {}

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
		return a->f() > b->f();
	}

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

	void AStar::Init(const SearchParam &param)
	{
		total_row_ = param.total_row;
		total_col_ = param.total_col;
		query_func_ = param.can_reach;
		map_size_ = total_row_ * total_col_;

		if (!maps_index_.empty())
		{
			memset(&maps_index_[0], 0, sizeof(decltype(maps_index_)::value_type) * maps_index_.size());
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

	inline Node* AStar::ExistInOpenList(const Point &point)
	{
		Node *node_ptr = maps_index_[point.row * total_row_ + point.col];
		return node_ptr ? (node_ptr->state == IN_OPENLIST ? node_ptr : nullptr) : nullptr;
	}

	inline bool AStar::ExistInCloseList(const Point &point)
	{
		Node *node_ptr = maps_index_[point.row * total_row_ + point.col];
		return node_ptr ? node_ptr->state == IN_CLOSELIST : false;
	}

	bool AStar::IsCanReach(const Point &target_point)
	{
		return (target_point.col >= 0 && target_point.col < total_col_ && target_point.row >= 0 && target_point.row < total_row_) ? query_func_(target_point) : false;
	}

	bool AStar::IsCanReachAndInOpen(const Point &target_point)
	{
		if (target_point.col >= 0 && target_point.col < total_col_ && target_point.row >= 0 && target_point.row < total_row_)
		{
			return ExistInCloseList(target_point) ? false : query_func_(target_point);
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

		int row_index = current_point.row - 1;
		const int max_row = current_point.row + 1;
		const int max_col = current_point.col + 1;

		while (row_index <= max_row)
		{
			int col_index = current_point.col - 1;
			while (col_index <= max_col)
			{
				if (IsCanReached(current_point, target(row_index, col_index), allow_corner))
				{
					surround_point.push_back(target);
				}
				++col_index;
			}
			++row_index;
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

	int AStar::GetIndexInOpenList(Node *node)
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

	void AStar::HandleFoundNode(Node *current_point, Node *new_point)
	{
		unsigned int g_value = CalculG(current_point, new_point->pos);

		if (g_value < new_point->g)
		{
			new_point->g = g_value;
			new_point->parent = current_point;
			PercolateUp(GetIndexInOpenList(new_point));
		}
	}

	void AStar::HandleNotFoundNode(Node *current_point, Node *new_point, const Point &end_point)
	{
		new_point->parent = current_point;
		new_point->g = CalculG(current_point, new_point->pos);
		new_point->h = CalculH(new_point->pos, end_point);

		Node *&node_ptr = maps_index_[new_point->pos.row * total_row_ + new_point->pos.col];
		node_ptr = new_point;
		node_ptr->state = IN_OPENLIST;

		open_list_.push_back(new_point);
		std::push_heap(open_list_.begin(), open_list_.end(), CompHeap);
	}

	bool AStar::InvalidParam(const SearchParam &param)
	{
		return (!param.can_reach
				|| (param.total_col < 0 || param.total_row < 0)
				|| (param.start_point.col < 0 || param.start_point.col >= param.total_col)
				|| (param.start_point.row < 0 || param.start_point.row >= param.total_row)
				|| (param.end_point.col < 0 || param.end_point.col >= param.total_col)
				|| (param.end_point.row < 0 || param.end_point.row >= param.total_row)
				);
	}

	std::vector<Point> AStar::Search(const SearchParam &param)
	{
		std::vector<Point> search_path;
		if (!InvalidParam(param))
		{
			Init(param);

			std::vector<Point> around_point;
			around_point.reserve(param.allow_corner ? 8 : 4);

			Node *start_node = new Node(param.start_point);
			open_list_.push_back(start_node);

			Node *&node_ptr = maps_index_[start_node->pos.row * total_row_ + start_node->pos.col];
			node_ptr = start_node;
			node_ptr->state = IN_OPENLIST;

			while (!open_list_.empty())
			{
				Node *current_point = *open_list_.begin();
				std::pop_heap(open_list_.begin(), open_list_.end(), CompHeap);
				open_list_.pop_back();
				maps_index_[current_point->pos.row * total_row_ + current_point->pos.col]->state = IN_CLOSELIST;

				SearchCanReached(current_point->pos, param.allow_corner, around_point);

				unsigned int index = 0;
				const unsigned int size = around_point.size();

				while (index < size)
				{
					Node *new_point = ExistInOpenList(around_point[index]);
					if (new_point)
					{
						HandleFoundNode(current_point, new_point);
					}
					else
					{
						new_point = new Node(around_point[index]);
						HandleNotFoundNode(current_point, new_point, param.end_point);

						if (around_point[index] == param.end_point)
						{
							while (new_point->parent)
							{
								search_path.push_back(new_point->pos);
								new_point = new_point->parent;
							}
							std::reverse(search_path.begin(), search_path.end());
							goto __end__;
						}
					}
					++index;
				}
			}
		__end__:
			Clear();
		}
		else
		{
			Clear();
			throw std::exception("invalid param!");
		}

		return search_path;
	}
}