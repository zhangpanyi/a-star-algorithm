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