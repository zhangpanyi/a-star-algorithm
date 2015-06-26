/**
 * A* 算法
 * author: zhangpanyi@live.com
 * https://github.com/zhangpanyi/AStar
 */

#pragma once

#include <vector>
#include <functional>
#include "Misc/NonCopyable.h"

namespace a_star
{
	struct Node;

	struct Vec2
	{
		unsigned short row;
		unsigned short col;

		Vec2() : row(0), col(0) {}
		Vec2(unsigned short row, unsigned short col) : row(row), col(col) {}

		bool operator== (const Vec2 &that) const
		{
			return row == that.row && col == that.col;
		}

		const Vec2& operator() (unsigned short row, unsigned short col)
		{
			this->row = row;
			this->col = col;
			return *this;
		}
	};

	typedef std::function<bool(const Vec2&)> QueryCallBack;

	struct AStarParam
	{
		bool			allow_corner;
		unsigned short	total_row;
		unsigned short	total_col;
		Vec2			start_point;
		Vec2			end_point;	
		QueryCallBack	is_can_reach;
		AStarParam() : total_row(0), total_col(0), is_can_reach(nullptr), allow_corner(false) {}
	};

	/**
	 * A-Star algorithm
	 */
	class AStar : public NonCopyable
	{
	public:
		AStar();
		~AStar();

	public:
		std::vector<Vec2> Search(const AStarParam &param);

	private:
		void Clear();

		void Init(const AStarParam &param);

		bool InvalidParam(const AStarParam &param);

	private:
		void PercolateUp(int hole);

		int GetNodeIndex(Node *node);

		bool HasNodeInCloseList(const Vec2 &point);

		bool HasNodeInOpenList(const Vec2 &point, Node *&out);

		bool IsCanReach(const Vec2 &point);

		bool IsCanReach(const Vec2 &current, const Vec2 &target, bool allow_corner);

		void SearchCanReachThePosition(const Vec2 &current, bool allow_corner, std::vector<Vec2> &can_each_pos);

		unsigned int CalculG(Node *parent, const Vec2 &current);

		unsigned int CalculH(const Vec2 &current, const Vec2 &end_point);

		void HandleFoundNode(Node *current, Node *target);

		void HandleNotFoundNode(Node *current, Node *target, const Vec2 &end_point);

	private:
		unsigned short		total_row_;
		unsigned short		total_col_;
		unsigned int		map_size_;
		QueryCallBack		query_func_;
		std::vector<Node *>	open_list_;
		std::vector<Node *>	maps_index_;
	};
}