#pragma once

#if __cplusplus
extern "C" {
#endif

#include "lua.h"
#include "lauxlib.h"

    int luaopen_FmodPlayer(lua_State* L);

#if __cplusplus
}
#endif
