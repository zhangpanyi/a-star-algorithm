/**
 * A* 算法
 * author: zhangpanyi@live.com
 * https://github.com/zhangpanyi/a-star.git
 */

#ifndef __ASTAR_H__
#define __ASTAR_H__

#include <vector>
#include <memory>
#include <cstdint>
#include <functional>

class BlockAllocator;

class AStar
{
public:
    /**
     * 二维向量
     */
    struct Vec2
    {
        uint16_t x;
        uint16_t y;

        Vec2() : x(0) , y(0)
        {
        }

        Vec2(uint16_t _x, uint16_t _y) : x(_x), y(_y)
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

    typedef std::function<bool(const Vec2&)> Callback;

    /**
     * 搜索参数
     */
    struct Param
    {
        bool        corner;     // 允许拐角
        uint16_t    height;     // 地图高度
        uint16_t    width;      // 地图宽度
        Vec2        start;      // 起点坐标
        Vec2        end;        // 终点坐标
        Callback    can_reach;  // 是否可通行

        Param()
            : height(0), width(0), corner(false)
        {
        }
    };

private:
    /**
     * 路径节点状态
     */
    enum NodeState
    {
        NOTEXIST,               // 不存在
        IN_OPENLIST,            // 在开启列表
        IN_CLOSELIST            // 在关闭列表
    };

    /**
     * 路径节点
     */
    struct Node
    {
        uint16_t    g;          // 与起点距离
        uint16_t    h;          // 与终点距离
        Vec2        pos;        // 节点位置
        NodeState   state;      // 节点状态
        Node*       parent;     // 父节点

        /**
         * 计算f值
         */
        int f() const
        {
            return g + h;
        }

        inline Node(const Vec2 &pos)
            : g(0), h(0), pos(pos), parent(nullptr), state(NOTEXIST)
        {
        }
    };

public:
    AStar(BlockAllocator *allocator);

    ~AStar();

public:
    /**
     * 获取直行估值
     */
    int get_step_value() const;

    /**
     * 获取拐角估值
     */
    int get_oblique_value() const;

    /**
     * 设置直行估值
     */
    void set_step_value(int value);

    /**
     * 获取拐角估值
     */
    void set_oblique_value(int value);

    /**
     * 执行寻路操作
     */
    std::vector<Vec2> find(const Param &param);

private:
    /**
     * 清理参数
     */
    void clear();

    /**
     * 初始化参数
     */
    void init_param(const Param &param);

    /**
     * 参数是否有效
     */
    bool is_vlid_param(const Param &param);

private:
    /**
     * 二叉堆上滤
     */
    void percolate_up(size_t hole);

    /**
     * 获取节点索引
     */
    bool get_node_index(Node *node, size_t &index);

    /**
     * 计算G值
     */
    uint16_t calcul_g_value(Node *parent_node, const Vec2 &current_pos);

    /**
     * 计算F值
     */
    uint16_t calcul_h_value(const Vec2 &current_pos, const Vec2 &end_pos);

    /**
     * 节点是否存在于开启列表
     */
    bool has_noode_in_open_list(const Vec2 &pos, Node *&out);

    /**
     * 节点是否存在于关闭列表
     */
    bool has_node_in_close_list(const Vec2 &pos);

    /**
     * 是否可到达
     */
    bool can_reach(const Vec2 &pos);

    /**
     * 当前点是否可到达目标点
     */
    bool can_reach(const Vec2 &current_pos, const Vec2 &target_pos, bool allow_corner);

    /**
     * 查找附近可到达的位置
     */
    void find_can_reach_pos(const Vec2 &current_pos, bool allow_corner, std::vector<Vec2> &lists);

    /**
     * 处理找到节点的情况
     */
    void handle_found_node(Node *current_node, Node *target_node);

    /**
     * 处理未找到节点的情况
     */
    void handle_not_found_node(Node *current_node, Node *target_node, const Vec2 &end_pos);

private:
    int                 step_value_;
    int                 oblique_value_;
    std::vector<Node *> map_;
    uint16_t            height_;
    uint16_t            width_;
    Callback            query_cb_;
    std::vector<Node *> open_list_;
    BlockAllocator*     allocator_;
};

#endif
