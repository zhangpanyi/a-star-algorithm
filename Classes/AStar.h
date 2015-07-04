/**
 * A* 算法
 * author: zhangpanyi@live.com
 * https://github.com/zhangpanyi/AStar
 */
#pragma once

#include <vector>
#include <cstdint>
#include <functional>
#include "Misc/NonCopyable.h"
#include "Misc/BlockAllocator.h"

/**
 * A-Star algorithm
 */
class AStar : public NonCopyable
{
public:
	struct Location
	{
		uint16_t row;
		uint16_t col;

		Location() : row(0), col(0) {}
		Location(uint16_t row, uint16_t col) : row(row), col(col)
		{}

		Location& Reset(uint16_t row, uint16_t col)
		{
			this->row = row;
			this->col = col;
			return *this;
		}

		bool operator== (const Location &that) const
		{
			return row == that.row && col == that.col;
		}
	};

	typedef std::function<bool(const Location&)> QueryCallBack;

	struct Param
	{
		bool			corner;
		uint16_t		height;
		uint16_t		width;
		Location		start;
		Location		end;
		QueryCallBack	is_canreach;
		Param() : height(0), width(0), is_canreach(nullptr), corner(false) {}
	};

private:
	enum NodeState
	{
		NOTEXIST,
		IN_OPENLIST,
		IN_CLOSELIST
	};

	struct Node
	{
		uint16_t		g;
		uint16_t		h;
		Location		pos;
		NodeState		state;
		Node*			parent;

		int f() const { return g + h; }

		inline Node(const Location &pos) : g(0), h(0), pos(pos), parent(nullptr), state(NOTEXIST) {}

		void* operator new(std::size_t size)
		{
			return SOA::GetInstance()->Allocate(size);
		}

			void operator delete(void* p) throw()
		{
			if (p) SOA::GetInstance()->Free(p, sizeof(Node));
		}
	};

public:
	AStar();
	~AStar();

public:
	std::vector<Location> Search(const Param &param);

private:
	void Clear();

	void Init(const Param &param);

	bool VlidParam(const Param &param);

private:
	void PercolateUp(size_t hole);

	bool GetIndexByNode(Node *node, size_t &index);

	bool HasNodeInCloseList(const Location &point);

	bool HasNodeInOpenList(const Location &point, Node *&out);

	bool IsCanReach(const Location &point);

	bool IsCanReach(const Location &current, const Location &target, bool allow_corner);

	void SearchCanReach(const Location &current, bool allow_corner, std::vector<Location> &out);

	unsigned int CalculG(Node *parent, const Location &current);

	unsigned int CalculH(const Location &current, const Location &end_point);

	void HandleFoundNode(Node *current, Node *target);

	void HandleNotFoundNode(Node *current, Node *target, const Location &end_point);

private:
	uint16_t				map_height_;
	uint16_t				map_width_;
	unsigned int			map_size_;
	QueryCallBack			query_func_;
	std::vector<Node *>		open_list_;
	std::vector<Node *>		maps_index_;
};