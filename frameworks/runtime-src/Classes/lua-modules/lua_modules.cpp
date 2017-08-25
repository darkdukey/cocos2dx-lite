
#include "lua_modules.h"

#if __cplusplus
extern "C" {
#endif

extern int luaopen_lpeg(lua_State *L);
extern int luaopen_struct(lua_State *L);
extern int luaopen_bit(lua_State *L);
extern int luaopen_protobuf_c(lua_State *L);
extern int luaopen_pack(lua_State *L);
    extern int luaopen_FmodPlayer(lua_State* L);

#if __cplusplus
} // extern "C"
#endif

static luaL_Reg modules[] = {
    { "lpeg", luaopen_lpeg },
    { "protobuf.c", luaopen_protobuf_c },
    { "pack", luaopen_pack },
    { "fmod", luaopen_FmodPlayer },

    { NULL, NULL }
};

void preload_lua_modules(lua_State *L)
{
    // load extensions
    luaL_Reg* lib = modules;
    lua_getglobal(L, "package");
    lua_getfield(L, -1, "preload");
    for (; lib->func; lib++)
    {
        lua_pushcfunction(L, lib->func);
        lua_setfield(L, -2, lib->name);
    }
    lua_pop(L, 2);
}
