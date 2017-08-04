#include "base/ccConfig.h"

#if CC_USE_CCS > 0

#ifndef __cocos2dx_csloader_h__
#define __cocos2dx_csloader_h__

#ifdef __cplusplus
extern "C" {
#endif
#include "tolua++.h"
#ifdef __cplusplus
}
#endif

int register_all_cocos2dx_csloader(lua_State* tolua_S);

#endif // CC_USE_CCS

















#endif // __cocos2dx_csloader_h__
