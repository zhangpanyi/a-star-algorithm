#include <string>
#include "AStar.h"
#include "wrap_AStar.h"
#include "../LuaFunction.hpp"

#define AStarParamMETA  "astar.Param"

static inline AStar::Param** toParamp(lua_State* L){
    return (AStar::Param**)luaL_checkudata(L, 1, AStarParamMETA);
}

AStar::Param* toParam(lua_State* L) {
    auto w = toParamp(L);
    if (*w == NULL)
        luaL_error(L, "Param already closed");
    return *w;
}

static int Param_create(lua_State* L)
{
    AStar::Param** w = (AStar::Param**)lua_newuserdata(L, sizeof(*w));

    *w = new AStar::Param();

    luaL_getmetatable(L, AStarParamMETA);
    lua_setmetatable(L, -2);

    return 1;
}

static int Param_gc(lua_State* L)
{
    auto w = toParamp(L);

    printf("finalizing LUA object (%s)\n", AStarParamMETA);

    if (!*w)
        return 0;

    delete *w;
    *w = nullptr; // mark as closed
    return 0;
}

static int Param_tostring(lua_State* L)
{
    auto w = toParamp(L);
    if (*w)
        lua_pushfstring(L, "Param (%p)", *w);
    else
        lua_pushliteral(L, "Param (closed)");
    return 1;
}

static int Param_getSize(lua_State* L) {
    auto w = toParam(L);
    if(w) {
        lua_pushinteger(L, w->width);
        return 1;
    }

    return 0;
}

static int Param_setSize(lua_State* L) {
    auto w = toParam(L);
    auto width = luaL_checkinteger(L, 2);
    auto height = luaL_checkinteger(L, 3);
    if(w) {
        w->height = height;
        w->width = width;
    }

    return 0;
}

static int Param_getCorner(lua_State* L) {
    auto w = toParam(L);
    if(w) {
        lua_pushboolean(L, w->corner);
        return 1;
    }

    return 0;
}

static int Param_setCorner(lua_State* L) {
    auto w = toParam(L);
    bool corner = lua_toboolean(L, 2);
    if(w) {
        w->corner = corner;
    }

    return 0;
}

static int Param_getStart(lua_State* L) {
    auto w = toParam(L);
    if(w) {
        lua_pushinteger(L, w->start.x);
        lua_pushinteger(L, w->start.y);

        return 2;
    }

    return 0;
}

static int Param_setStart(lua_State* L) {
    auto w = toParam(L);
    auto x = lua_tointeger(L, 2);
    auto y = lua_tointeger(L, 3);
    if(w) {
        w->start.x = x;
        w->start.y = y;
    }

    return 0;
}


static int Param_getEnd(lua_State* L) {
    auto w = toParam(L);
    if(w) {
        lua_pushinteger(L, w->end.x);
        lua_pushinteger(L, w->end.y);

        return 2;
    }

    return 0;
}

static int Param_setEnd(lua_State* L) {
    auto w = toParam(L);
    auto x = lua_tointeger(L, 2);
    auto y = lua_tointeger(L, 3);
    if(w) {
        w->start.x = x;
        w->start.y = y;
    }

    return 0;
}

static bool cacheReturn = false;
static LuaFunction* _call = nullptr;
static int Param_setQueryFunc(lua_State* L) {
    if (_call) {
      delete _call;
      _call = nullptr;
    }

    auto w = toParam(L);
    _call = new LuaFunction(L, 2);
    _call->setReturnCnt(1);
    _call->setCheckReturnFunction([](lua_State* L){
      luaL_argcheck(L, lua_isboolean(L, -1), -1, "'boolean' expected");
		  cacheReturn = lua_toboolean(L, -1);
    });

    w->can_reach = [L](const AStar::Vec2 &pos)->bool {
      int x = (int)pos.x;
      int y = (int)pos.y;
      (*_call)(x, y);
      return cacheReturn;
    };

    return 0;
}

static int Param_find(lua_State* L) {
  static AStar as;
  auto w = toParam(L);
  auto paths = as.find(*w);

  lua_createtable(L, paths.size(), 0);
  for (size_t i = 0; i < paths.size(); ++i)
  {
      lua_createtable(L, 0, 2);
      lua_pushinteger(L, paths[i].x);
      lua_rawseti(L, -2, 1);
      lua_pushinteger(L, paths[i].y);
      lua_rawseti(L, -2, 2);
      lua_rawseti(L, -2, i + 1);
  }
  return 1;
}

static luaL_Reg methods[] = {
    { "__gc", Param_gc },
    { "__tostring", Param_tostring },
    { "getSize", Param_getSize },
    { "setSize", Param_setSize },
    { "getCorner", Param_getCorner },
    { "setCorner", Param_setCorner },
    { "getStart", Param_getStart },
    { "setStart", Param_setStart },
    { "getEnd", Param_getEnd },
    { "setEnd", Param_setEnd },
    { "setQueryFunc", Param_setQueryFunc },

    { NULL, NULL },
};

static luaL_Reg api[] = {
    { "Param", Param_create },
    { "find", Param_find },

    { NULL, NULL },
};

int luaopen_astar(lua_State *L)
{
    // create the class Metatable
    luaL_newmetatable(L, AStarParamMETA);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, methods);
    lua_pop(L, 1);

    // register the net api
    lua_newtable(L);
    luaL_register(L, NULL, api);
	return 1;
}
