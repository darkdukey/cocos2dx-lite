#include "base/ccConfig.h"

#if CC_USE_SIMPLE_AUDIO > 0

#ifndef __cocos2dx_cocosdenshion_h__
#define __cocos2dx_cocosdenshion_h__

#ifdef __cplusplus
extern "C" {
#endif
#include "tolua++.h"
#ifdef __cplusplus
}
#endif

int register_all_cocos2dx_cocosdenshion(lua_State* tolua_S);

#endif // CC_USE_SIMPLE_AUDIO

























#endif // __cocos2dx_cocosdenshion_h__
