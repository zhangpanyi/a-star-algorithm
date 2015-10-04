/**
 * A* 算法
 * author: zhangpanyi@live.com
 * https://github.com/zhangpanyi/a-star.git
 */
#pragma once

#include <vector>
#include <cstdint>
#include <functional>
#include "block_allocator.h"

/**
 * A-Star algorithm
 */
class a_star
{
public:
	/**
	 * 二维坐标
	 */
	struct Vec2
	{
		uint16_t x;
		uint16_t y;

		Vec2()
			: x(0)
			, y(0)
		{
		}

		Vec2(uint16_t _x, uint16_t _y)
			: x(_x)
			, y(_y)
		{
		}

		void set(uint16_t x, uint16_t y)
		{
			this->x = x;
			this->y = y;
		}

		bool operator== (const Vec2 &that) const
		{
			return x == that.x && y == that.y;
		}
	};

	typedef std::function<bool(const Vec2&)> query_function;

	/**
	 * 搜索参数
	 */
	struct Param
	{
		bool			allow_corner;
		uint16_t		height;
		uint16_t		width;
		Vec2			start;
		Vec2			end;
		query_function	is_canreach;

		Param()
			: height(0)
			, width(0)
			, is_canreach(nullptr)
			, allow_corner(false)
		{
		}

		Param(const Vec2 &_start, const Vec2 &_end, uint16_t _width, uint16_t _height, const query_function &_is_canreach, bool _allow_corner)
			: start(_start)
			, end(_end)
			, width(_width)
			, height(_height)
			, is_canreach(_is_canreach)
			, allow_corner(_allow_corner)
		{

		}
	};

private:
	/**
	 * 节点对象分配器
	 */
	class soa : public block_allocator, public singleton< soa >
	{
		soa() = default;
		~soa() = default;
		friend class singleton< soa >;
	};

	/**
	 * 路径节点状态
	 */
	enum NodeState
	{
		NOTEXIST,
		IN_OPENLIST,
		IN_CLOSELIST
	};

	/**
	 * 路径节点
	 */
	struct Node
	{
		uint16_t		g;
		uint16_t		h;
		Vec2			pos;
		NodeState		state;
		Node*			parent;

		int f() const
		{
			return g + h;
		}

		inline Node(const Vec2 &pos)
			: g(0)
			, h(0)
			, pos(pos)
			, parent(nullptr)
			, state(NOTEXIST)
		{
		}

		void* operator new(std::size_t size)
		{
			return soa::instance()->allocate(size);
		}

		void operator delete(void* p) throw()
		{
			soa::instance()->free(p, sizeof(Node));
		}
	};

public:
	a_star();
	~a_star();

public:
	std::vector<Vec2> search(const Param &param);

private:
	void clear();

	void init(const Param &param);

	bool vlid_param(const Param &param);

private:
	void percolate_up(size_t hole);

	bool get_node_index(Node *node, size_t &index);

	uint16_t calcul_g_value(Node *parent_node, const Vec2 &current_pos);

	uint16_t calcul_h_value(const Vec2 &current_pos, const Vec2 &end_pos);

	bool has_node_in_open_list(const Vec2 &pos, Node *&out);

	bool has_node_in_close_list(const Vec2 &pos);

	bool is_can_arrive(const Vec2 &pos);

	bool is_can_arrive(const Vec2 &current_pos, const Vec2 &target_pos, bool allow_corner);

	void find_can_arrive_pos(const Vec2 &current_pos, bool allow_corner, std::vector<Vec2> &can_arrive_pos);

	void handle_found_node(Node *current_node, Node *target_node);

	void handle_not_found_node(Node *current_node, Node *target_node, const Vec2 &end_pos);

private:
	std::vector<Node *>		maps_;
	uint16_t				height_;
	uint16_t				width_;
	query_function			query_function_;
	std::vector<Node *>		open_list_;
};