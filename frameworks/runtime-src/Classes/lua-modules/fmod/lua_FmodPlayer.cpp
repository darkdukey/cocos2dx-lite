#include "lua_FmodPlayer.hpp"
#include "FmodPlayer.h"

static void stackDump(lua_State* L)
{
    int i = 1;
    int top = lua_gettop(L);
    for (i = 1; i <= top; i++)
    {
        int type = lua_type(L, i);
        switch (type)
        {
        case LUA_TSTRING:
            printf("%s", lua_tostring(L, i));
            break;
        case LUA_TBOOLEAN:
            printf(lua_toboolean(L, i) ? "true" : "false");
            break;
        case LUA_TNUMBER:
            printf("%g", lua_tonumber(L, i));
            break;
        default:
            printf("%s", lua_typename(L, type));
            break;
        }
        printf("  ");
    }
    printf("\n");
}

static int playBackgroundMusic(lua_State* pL)
{
    const char* filename = luaL_checkstring(pL, 1);
    bool loop = lua_toboolean(pL, 2);
    FmodPlayer::getInstance()->playBackgroundMusic(filename,loop);
    return 0;
}

static int stopBackgroundMusic(lua_State* pL)
{
    FmodPlayer::getInstance()->stopBackgroundMusic();
    return 0;
}

static int isBackgroundMusicPlaying(lua_State* pL)
{
    bool playing = FmodPlayer::getInstance()->isBackgroundMusicPlaying();
    lua_pushboolean(pL, playing);
    return 1;
}

static int getBackgroundMusicVolume(lua_State* pL)
{
    float volume = FmodPlayer::getInstance()->getBackgroundMusicVolume();
    lua_pushnumber(pL,volume);
    return 1;
}

static int setBackgroundMusicVolume(lua_State* pL)
{
    float volume = luaL_checknumber(pL, 1);
    FmodPlayer::getInstance()->setBackgroundMusicVolume(volume);
    return 0;
}

static int pauseBackgroundMusic(lua_State* pL)
{
    FmodPlayer::getInstance()->pauseBackgroundMusic();
    return 0;
}

static int resumeBackgroundMusic(lua_State* pL)
{
    FmodPlayer::getInstance()->resumeBackgroundMusic();
    return 0;
}

static int playEffect(lua_State* pL)
{
    const char* filename = luaL_checkstring(pL, -1);
    bool loop = lua_toboolean(pL, 2);
    FmodPlayer::getInstance()->playEffect(filename,loop);
    return 0;
}

static int stopAllEffects(lua_State* pL)
{
    FmodPlayer::getInstance()->stopAllEffects();
    return 0;
}

static int getEffectsVolume(lua_State* pL)
{
    float volume = FmodPlayer::getInstance()->getEffectsVolume();
    lua_pushnumber(pL, volume);
    return 1;
}

static int setEffectsVolume(lua_State* pL)
{
    float volume = luaL_checknumber(pL, 1);
    FmodPlayer::getInstance()->setEffectsVolume(volume);
    return 0;
}

static const struct luaL_reg FmodPlayerLib[] =
{
    { "playBackgroundMusic", playBackgroundMusic },
    { "stopBackgroundMusic", stopBackgroundMusic },
    { "isBackgroundMusicPlaying", isBackgroundMusicPlaying },
    { "getBackgroundMusicVolume", getBackgroundMusicVolume },
    { "setBackgroundMusicVolume", setBackgroundMusicVolume },
    { "pauseBackgroundMusic", pauseBackgroundMusic },
    { "resumeBackgroundMusic", resumeBackgroundMusic },
    { "playEffect", playEffect },
    { "stopAllEffects", stopAllEffects },
    { "getEffectsVolume", getEffectsVolume },
    { "setEffectsVolume", setEffectsVolume },
    { NULL, NULL }
};

int luaopen_FmodPlayer(lua_State* pL)
{
    luaL_openlib(pL, "FmodPlayer", FmodPlayerLib, 0);
    return 1;
}
