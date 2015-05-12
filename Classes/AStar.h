#pragma once

#include <deque>
#include <vector>
#include "AStar/Types.h"
#include "AStar/NonCopyable.h"

namespace pathfinding
{
	struct Node;

	class AStar : public NonCopyable
	{
	public:
		AStar();
		~AStar();

	public:
		/**
		 * 执行A*搜索
		 * @param param 搜索参数
		 * @return std::deque<Point> 搜索路径
		 */
		std::deque<Point> Search(const SearchParam &param);

	private:
		/**
		 * 清理
		 */
		void Clear();

		/**
		 * 初始化
		 * @param param 搜索参数
		 */
		void Init(const SearchParam &param);

		/**
		 * 检测参数是否无效
		 * @param param 搜索参数
		 */
		bool InvalidParam(const SearchParam &param);

	private:
		/**
		 * 点是否存在于开启列表
		 * @param point 当前点
		 * @return 存在返回格子结点的指针，不存在返回nullptr
		 */
		Node* ExistInOpenList(const Point &point);

		/**
		 * 点是否存在于关闭列表
		 * @param point 当前点
		 * @return 存在返回true，不存在返回false
		 */
		bool ExistInCloseList(const Point &point);

		/**
		 * 查询点是否可通行
		 * @param target_point 目标点
		 * @return 成功返回true，失败返回false
		 */
		bool IsCanReach(const Point &target_point);
		bool IsCanReachAndInOpen(const Point &target_point);

		/**
		 * 查询点是否可到达
		 * @param current_point 当前点
		 * @param target_point 目标点
		 * @param allow_corner 是否允许拐角
		 * @return 成功返回true，失败返回false
		 */
		bool IsCanReached(const Point &current_point, const Point &target_point, bool allow_corner);

		/**
		 * 搜索周围可通行的点
		 * @param current_point 当前点
		 * @param allow_corner 是否允许拐角
		 * @param surround_point 存放搜索结果的数组
		 */
		void SearchCanReached(const Point &current_point, bool allow_corner, std::vector<Point> &surround_point);

		/**
		 * 计算G值
		 * @param parent 父节点
		 * @param current_point 当前点
		 */
		unsigned int CalculG(Node *parent, const Point &current_point);

		/**
		 * 计算H值
		 * @param current_point 当前点
		 * @param end_point 终点
		 */
		unsigned int CalculH(const Point &current_point, const Point &end_point);

		/**
		 * 获取节点在开启列表中的索引值
		 * @return 失败返回-1
		 */
		int GetIndex(Node *node);

		/**
		 * 开启列表上滤(二叉堆上滤)
		 * @param hole 上滤位置
		 */
		void PercolateUp(int hole);

	private:
		/**
		 * 当节点存在于开启列表中的处理函数
		 */
		void HandleFoundNode(Node *current_point, Node *new_point);

		/**
		 * 当节点不存在于开启列表中的处理函数
		 */
		void HandleNotFoundNode(Node *current_point, Node *new_point, const Point &end_point);

	private:
		unsigned short		total_row_;			// 地图行数
		unsigned short		total_col_;			// 地图列数
		unsigned int		map_size_;			// 地图大小
		QueryCallBack		query_func_;		// 查询函数
		std::vector<Node *>	open_list_;			// 开启列表
		std::vector<Node *>	maps_index_;		// 地图索引
	};
}