#pragma once

#include <functional>

namespace pathfinding
{
	/**
	 * 位置
	 * 用于表示具体位置点
	 */
	struct Point
	{
		unsigned short row;
		unsigned short col;

		Point() : row(0), col(0) {}
		Point(unsigned short row, unsigned short col) : row(row), col(col) {}

		bool operator== (const Point &that) const
		{
			return row == that.row && col == that.col;
		}

		const Point& operator() (unsigned short row, unsigned short col)
		{
			this->row = row;
			this->col = col;
			return *this;
		}
	};

	/**
	 * 查询回调函数
	 * 用于查询路径是否可通行
	 */
	typedef std::function<bool(const Point&)> QueryCallBack;

	/**
	 * 搜索参数
	 * 向寻路算法提供的参数
	 */
	struct SearchParam
	{
		bool			allow_corner;		// 允许拐角
		unsigned short	total_row;			// 地图总行数
		unsigned short	total_col;			// 地图总列数
		Point			start_point;		// 起点位置
		Point			end_point;			// 终点位置
		QueryCallBack	can_reach;			// 查询回调函数
		SearchParam() : total_row(0), total_col(0), can_reach(nullptr), allow_corner(false) {}
	};
}