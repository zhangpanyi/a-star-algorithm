#ifndef __WRAP_ASTAR_H__
#define __WRAP_ASTAR_H__

#if __cplusplus
extern "C" {
#endif
    
#include "lua.h"
#include "lauxlib.h"

int luaopen_astar(lua_State *L);
    
#if __cplusplus
}
#endif


#endif