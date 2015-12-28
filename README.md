# A-Star Algorithm
这是使用C++实现的高效的A-Star算法。只对算法的程序实现做了尽力而为的优化，并没有对算法自身进行改良。优化措施主要在于：快速判断路径节点是否在开启/关闭列表中、快速查找最小f值的节点以及优化路径节点频繁分配内存的问题。算法已经在[这款三消类游戏](https://github.com/zhangpanyi/eliminate-game)中得到运用。

# 运行环境
支持c++11的编译器

# 使用示例
```c++
// 搜索参数
AStar::Param param;
param.width = 10;
param.height = 10;
param.allow_corner = false;
param.start = AStar::Vec2(0, 0);
param.end = AStar::Vec2(9, 9);
param.is_canreach = [&](const AStar::Vec2 &pos)->bool
{
    return maps[pos.y][pos.x] == 0;
};

// 执行搜索
AStar object;
std::deque<AStar::Vec2> path = object.search(param);
```

# 效率测试
| 地图大小 | 允许斜走 | 全程耗时 |
| ---- | ---- | ---- |
| 100 * 100 | right-aligned | $1600 |
| 100 * 100 | centered      |   $12 |
| 100 * 100 | are neat      |    $1 |

# 实现思路
[http://igameday.com/2015/12/08/1.html](http://igameday.com/2015/12/08/1.html)
