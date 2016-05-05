#ifndef __WRAP_ASTAR_H__
#define __WRAP_ASTAR_H__

struct lua_State;

extern "C"
{
	int luaopen_astar(lua_State *L);
}

#endif
