# A-Star Algorithm
这是使用C++实现的高效的A-Star算法。只对算法的程序实现做了尽力而为的优化，并没有对算法自身进行改良。优化措施主要在于：快速判断路径节点是否在开启/关闭列表中、快速查找最小f值的节点以及优化路径节点频繁分配内存的问题。算法已经在[这款三消类游戏](https://github.com/zhangpanyi/eliminate-game)中得到运用。

# 运行环境
支持c++11的编译器

# 使用示例
```c++
char maps[10][10] =
{
	{ 0, 1, 0, 0, 0, 1, 0, 0, 0, 0 },
	{ 0, 0, 0, 1, 0, 1, 0, 1, 0, 1 },
	{ 1, 1, 1, 1, 0, 1, 0, 1, 0, 1 },
	{ 0, 0, 0, 1, 0, 0, 0, 1, 0, 1 },
	{ 0, 1, 0, 1, 1, 1, 1, 1, 0, 1 },
	{ 0, 1, 0, 0, 0, 0, 0, 0, 0, 1 },
	{ 0, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 0, 0, 0, 0, 1, 0, 0, 0, 1, 0 },
	{ 1, 1, 0, 0, 1, 0, 1, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 1, 0, 1, 0 },
};

// 搜索参数
AStar::Param param;
param.width = 10;
param.height = 10;
param.corner = false;
param.start = AStar::Vec2(0, 0);
param.end = AStar::Vec2(9, 9);
param.can_reach = [&](const AStar::Vec2 &pos)->bool
{
	return maps[pos.y][pos.x] == 0;
};

// 执行搜索
AStar as;
auto path = as.find(param);
```

# Lua的使用
```
local maps =
{
    0, 1, 0, 0, 0, 1, 0, 0, 0, 0,
    0, 0, 0, 1, 0, 1, 0, 1, 0, 1,
    1, 1, 1, 1, 0, 1, 0, 1, 0, 1,
    0, 0, 0, 1, 0, 0, 0, 1, 0, 1,
    0, 1, 0, 1, 1, 1, 1, 1, 0, 1,
    0, 1, 0, 0, 0, 0, 0, 0, 0, 1,
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 1, 0, 0, 0, 1, 0,
    1, 1, 0, 0, 1, 0, 1, 0, 0, 1,
    0, 0, 0, 0, 0, 0, 1, 0, 0, 0
}

function can_reach(x, y)
    idx = 1 + y * 10 + x
    return maps[idx] == 0
end

local param = AStarParam.new()
param:setSize(10, 10)
param:setCorner(false)
param:setStart(0, 0)
param:setEnd(9, 9)
param:setQueryFunc("can_reach")
local paths = AStarFind(param)

if #(paths) == 0 then
    print("find fail!")
else
    for i=1, #(paths) do      
        print(paths[i].x, paths[i].y)  
    end
end
```

# 效率测试
测试实在无阻碍地图上进行的，计算从左上角搜索到右下角所耗费的时间。与实际项目中可能有偏差。

| 地图大小 | 允许斜走 | 全程耗时 |
| ---- | ---- | ---- |
| 100 * 100 | 是| 0.000s |
| 100 * 100 | 否 | 0.001s |
| 1000 * 1000 | 是 | 0.004s |
| 1000 * 1000 | 否 | 0.091s |
