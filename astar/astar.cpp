#include "astar.h"
#include <cassert>
#include <cstring>
#include <algorithm>
#include "blockallocator.h"

static const int kStepValue = 10;
static const int kObliqueValue = 14;

AStar::AStar(BlockAllocator *allocator)
    : width_(0)
    , height_(0)
    , allocator_(allocator)
    , step_val_(kStepValue)
    , oblique_val_(kObliqueValue)
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
    return step_val_;
}

// 获取拐角估值
int AStar::get_oblique_value() const
{
    return oblique_val_;
}

// 设置直行估值
void AStar::set_step_value(int value)
{
    step_val_ = value;
}

// 获取拐角估值
void AStar::set_oblique_value(int value)
{
    oblique_val_ = value;
}

// 清理参数
void AStar::clear()
{
    size_t index = 0;
    const size_t max_size = width_ * height_;
    while (index < max_size)
    {
        allocator_->free(mapping_[index++], sizeof(Node));
    }
    open_list_.clear();
    can_pass_ = nullptr;
    width_ = height_ = 0;
}

// 初始化操作
void AStar::init(const Params &param)
{
    width_ = param.width;
    height_ = param.height;
    can_pass_ = param.can_pass;
    if (!mapping_.empty())
    {
        memset(&mapping_[0], 0, sizeof(Node*) * mapping_.size());
    }
    mapping_.resize(width_ * height_, nullptr);
}

// 参数是否有效
bool AStar::is_vlid_params(const AStar::Params &param)
{
    return (param.can_pass != nullptr
            && (param.width > 0 && param.height > 0)
            && (param.end.x >= 0 && param.end.x < param.width)
            && (param.end.y >= 0 && param.end.y < param.height)
            && (param.start.x >= 0 && param.start.x < param.width)
            && (param.start.y >= 0 && param.start.y < param.height)
            );
}

// 获取节点索引
bool AStar::get_node_index(Node *node, size_t *index)
{
    *index = 0;
    const size_t size = open_list_.size();
    while (*index < size)
    {
        if (open_list_[*index]->pos == node->pos)
        {
            return true;
        }
        ++(*index);
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
inline uint16_t AStar::calcul_g_value(Node *parent, const Vec2 &current)
{
    uint16_t g_value = current.distance(parent->pos) == 2 ? oblique_val_ : step_val_;
    return g_value += parent->g;
}

// 计算F值
inline uint16_t AStar::calcul_h_value(const Vec2 &current, const Vec2 &end)
{
    unsigned int h_value = end.distance(current);
    return h_value * step_val_;
}

// 节点是否存在于开启列表
inline bool AStar::in_open_list(const Vec2 &pos, Node *&out_node)
{
    out_node = mapping_[pos.y * width_ + pos.x];
    return out_node ? out_node->state == IN_OPENLIST : false;
}

// 节点是否存在于关闭列表
inline bool AStar::in_closed_list(const Vec2 &pos)
{
    Node *node_ptr = mapping_[pos.y * width_ + pos.x];
    return node_ptr ? node_ptr->state == IN_CLOSEDLIST : false;
}

// 是否可到达
bool AStar::can_pass(const Vec2 &pos)
{
    return (pos.x >= 0 && pos.x < width_ && pos.y >= 0 && pos.y < height_) ? can_pass_(pos) : false;
}

// 当前点是否可到达目标点
bool AStar::can_pass(const Vec2 &current, const Vec2 &destination, bool allow_corner)
{
    if (destination.x >= 0 && destination.x < width_ && destination.y >= 0 && destination.y < height_)
    {
        if (in_closed_list(destination))
        {
            return false;
        }

        if (destination.distance(current) == 1)
        {
            return can_pass_(destination);
        }
        else if (allow_corner)
        {
            return can_pass_(destination) && (can_pass(Vec2(current.x + destination.x - current.x, current.y))
                    && can_pass(Vec2(current.x, current.y + destination.y - current.y)));
        }
    }
    return false;
}

// 查找附近可通过的节点
void AStar::find_can_pass_nodes(const Vec2 &current, bool corner, std::vector<Vec2> *out_lists)
{
    Vec2 destination;
    int row_index = current.y - 1;
    const int max_row = current.y + 1;
    const int max_col = current.x + 1;

    if (row_index < 0)
    {
        row_index = 0;
    }
    
    while (row_index <= max_row)
    {
        int col_index = current.x - 1;

        if (col_index < 0)
        {
            col_index = 0;
        }
    
        while (col_index <= max_col)
        {
            destination.reset(col_index, row_index);
            if (can_pass(current, destination, corner))
            {
                out_lists->push_back(destination);
            }
            ++col_index;
        }
        ++row_index;
    }
}

// 处理找到节点的情况
void AStar::handle_found_node(Node *current, Node *destination)
{
    unsigned int g_value = calcul_g_value(current, destination->pos);
    if (g_value < destination->g)
    {
        destination->g = g_value;
        destination->parent = current;

        size_t index = 0;
        if (get_node_index(destination, &index))
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
void AStar::handle_not_found_node(Node *current, Node *destination, const Vec2 &end)
{
    destination->parent = current;
    destination->h = calcul_h_value(destination->pos, end);
    destination->g = calcul_g_value(current, destination->pos);

    Node *&reference_node = mapping_[destination->pos.y * width_ + destination->pos.x];
    reference_node = destination;
    reference_node->state = IN_OPENLIST;

    open_list_.push_back(destination);
    std::push_heap(open_list_.begin(), open_list_.end(), [](const Node *a, const Node *b)->bool
    {
        return a->f() > b->f();
    });
}

// 执行寻路操作
std::vector<AStar::Vec2> AStar::find(const Params &param)
{
    std::vector<Vec2> paths;
    assert(is_vlid_params(param));
    if (!is_vlid_params(param))
    {
        return paths;
    }

    // 初始化
    init(param);
    std::vector<Vec2> nearby_nodes;
    nearby_nodes.reserve(param.corner ? 8 : 4);

    // 将起点放入开启列表
    Node *start_node = new(allocator_->allocate(sizeof(Node))) Node(param.start);
    open_list_.push_back(start_node);
    Node *&reference_node = mapping_[start_node->pos.y * width_ + start_node->pos.x];
    reference_node = start_node;
    reference_node->state = IN_OPENLIST;

    // 寻路操作
    while (!open_list_.empty())
    {
        // 找出f值最小节点
        Node *current = open_list_.front();
        std::pop_heap(open_list_.begin(), open_list_.end(), [](const Node *a, const Node *b)->bool
        {
            return a->f() > b->f();
        });
        open_list_.pop_back();
        mapping_[current->pos.y * width_ + current->pos.x]->state = IN_CLOSEDLIST;

        // 是否找到终点
        if (current->pos == param.end)
        {
            while (current->parent)
            {
                paths.push_back(current->pos);
                current = current->parent;
            }
            std::reverse(paths.begin(), paths.end());
            goto __end__;
        }

        // 查找周围可通过节点
        nearby_nodes.clear();
        find_can_pass_nodes(current->pos, param.corner, &nearby_nodes);

        // 计算周围节点的估值
        size_t index = 0;
        const size_t size = nearby_nodes.size();
        while (index < size)
        {
            Node *next_node = nullptr;
            if (in_open_list(nearby_nodes[index], next_node))
            {
                handle_found_node(current, next_node);
            }
            else
            {
                next_node = new(allocator_->allocate(sizeof(Node))) Node(nearby_nodes[index]);
                handle_not_found_node(current, next_node, param.end);
            }
            ++index;
        }
    }

__end__:
    clear();
    return paths;
}
