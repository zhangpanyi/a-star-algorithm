#include "lua.hpp"
#include <string>
#include "AStar.h"
#include "wrap_AStar.h"

struct AStarParam
{
	int width;
	int height;
	bool corner;
	AStar::Vec2 start;
	AStar::Vec2 end;
	std::string can_reach;

	AStarParam()
		: width(0)
		, height(0)
		, corner(false)
	{
	}

	~AStarParam() {}
};

static AStarParam* check_a_star_param(lua_State *L, int idx = -1)
{
	void *data = luaL_checkudata(L, idx, "meta_AStarParam");
	luaL_argcheck(L, data != nullptr, idx, "'AStarParam' expected");
	return reinterpret_cast<AStarParam *>(data);
}

static int a_star_param_new(lua_State *L)
{
	lua_newuserdata(L, sizeof(AStarParam));
	luaL_setmetatable(L, "meta_AStarParam");
	AStarParam *param = check_a_star_param(L);
	param->AStarParam::AStarParam();
	return 1;
}

static int a_star_param_gc(lua_State *L)
{
	AStarParam *param = check_a_star_param(L);
	param->AStarParam::~AStarParam();
	return 0;
}

static int a_star_param_get_size(lua_State *L)
{
	AStarParam *param = check_a_star_param(L);
	lua_createtable(L, 0, 2);
	lua_pushinteger(L, param->width);
	lua_setfield(L, -2, "width");
	lua_pushinteger(L, param->height);
	lua_setfield(L, -2, "height");
	return 1;
}

static int a_star_param_set_size(lua_State *L)
{
	AStarParam *param = check_a_star_param(L, 1);
	luaL_checkinteger(L, 2);
	luaL_checkinteger(L, 3);
	param->width = lua_tointeger(L, 2);
	param->height = lua_tointeger(L, 3);
	return 0;
}

static int a_star_param_get_corner(lua_State *L)
{
	AStarParam *param = check_a_star_param(L);
	lua_pushboolean(L, param->corner);
	return 1;
}

static int a_star_param_set_corner(lua_State *L)
{
	AStarParam *param = check_a_star_param(L, 1);
	luaL_argcheck(L, lua_isboolean(L, 2), 2, "'boolean' expected");
	param->corner = lua_toboolean(L, 2);
	return 0;
}

static int a_star_param_get_start(lua_State *L)
{
	AStarParam *param = check_a_star_param(L);
	lua_createtable(L, 0 ,2);
	lua_pushinteger(L, param->start.x);
	lua_setfield(L, -2, "x");
	lua_pushinteger(L, param->start.y);
	lua_setfield(L, -2, "y");
	return 1;
}

static int a_star_param_set_start(lua_State *L)
{
	AStarParam *param = check_a_star_param(L, 1);
	luaL_checkinteger(L, 2);
	luaL_checkinteger(L, 3);
	param->start.x = lua_tointeger(L, 2);
	param->start.y = lua_tointeger(L, 3);
	return 0;
}

static int a_star_param_get_end(lua_State *L)
{
	AStarParam *param = check_a_star_param(L);
	lua_createtable(L, 0, 2);
	lua_pushinteger(L, param->end.x);
	lua_setfield(L, -2, "x");
	lua_pushinteger(L, param->end.y);
	lua_setfield(L, -2, "y");
	return 1;
}

static int a_star_param_set_end(lua_State *L)
{
	AStarParam *param = check_a_star_param(L, 1);
	luaL_checkinteger(L, 2);
	luaL_checkinteger(L, 3);
	param->end.x = lua_tointeger(L, 2);
	param->end.y = lua_tointeger(L, 3);
	return 0;
}

static int a_star_param_is_can_reach(lua_State *L)
{
	AStarParam *param = check_a_star_param(L, 1);
	luaL_checkstring(L, 2);
	param->can_reach = lua_tostring(L, 2);
	return 0;
}

static int a_star_param_find(lua_State *L)
{
	AStarParam *l_param = check_a_star_param(L, 1);
	lua_getglobal(L, l_param->can_reach.c_str());

	AStar::Param param;
	param.width = l_param->width;
	param.height = l_param->height;
	param.corner = l_param->corner;
	param.start = l_param->start;
	param.end = l_param->end;
	param.can_reach = [&](const AStar::Vec2 &pos)->bool
	{
		lua_pushvalue(L, -1);
		lua_pushinteger(L, pos.x);
		lua_pushinteger(L, pos.y);
		lua_pcall(L, 2, 1, 0);
		luaL_argcheck(L, lua_isboolean(L, -1), -1, "'boolean' expected");
		bool ret = lua_toboolean(L, -1);
		lua_pop(L, 1);
		return ret;
	};

	static AStar instance;
	auto paths = instance.find(param);

	lua_createtable(L, paths.size(), 0);
	for (size_t i = 0; i < paths.size(); ++i)
	{
		lua_createtable(L, 0, 2);
		lua_pushinteger(L, paths[i].x);
		lua_setfield(L, -2, "x");
		lua_pushinteger(L, paths[i].y);
		lua_setfield(L, -2, "y");
		lua_rawseti(L, -2, i + 1);
	}

	return 1;
}

static const luaL_Reg lua_a_star_param_lib_f[] =
{
	{ "new", a_star_param_new },
	{ nullptr, nullptr }
};

static const luaL_Reg lua_a_star_param_lib_m[] =
{
	{ "getSize", a_star_param_get_size },
	{ "setSize", a_star_param_set_size },
	{ "getCorner", a_star_param_get_corner },
	{ "setCorner", a_star_param_set_corner },
	{ "getStart", a_star_param_get_start },
	{ "setStart", a_star_param_set_start },
	{ "getEnd", a_star_param_get_end },
	{ "setEnd", a_star_param_set_end },
	{ "setQueryFunc", a_star_param_is_can_reach },
	{ "__gc", a_star_param_gc },
	{ nullptr, nullptr }
};

int luaopen_a_star_param(lua_State *L)
{
	luaL_newmetatable(L, "meta_AStarParam");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, lua_a_star_param_lib_m, 0);

	luaL_newlib(L, lua_a_star_param_lib_f);

	return 1;
}

int luaopen_astar(lua_State *L)
{
	luaL_requiref(L, "AStarParam", luaopen_a_star_param, 1);
	lua_pushcfunction(L, a_star_param_find);
	lua_setglobal(L, "AStarFind");
	return 0;
}