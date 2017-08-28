LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := cocos2dlua_shared

LOCAL_MODULE_FILENAME := libcocos2dlua

LOCAL_SRC_FILES := \
../../Classes/AppDelegate.cpp \
../../Classes/ProjectConfig/ProjectConfig.cpp \
../../Classes/ProjectConfig/SimulatorConfig.cpp \
hellolua/main.cpp

FMOD_SRC_FILES := ../../Classes/lua-modules/fmod/FmodPlayer.cpp \
				  ../../Classes/lua-modules/fmod/lua_FmodPlayer.cpp
LOCAL_SRC_FILES += $(FMOD_SRC_FILES)

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes

# _COCOS_HEADER_ANDROID_BEGIN
# _COCOS_HEADER_ANDROID_END

LOCAL_STATIC_LIBRARIES := cocos2d_lua_static
# LOCAL_STATIC_LIBRARIES += cocosdenshion_static
LOCAL_STATIC_LIBRARIES += lua_modules_static
LOCAL_STATIC_LIBRARIES += cocos_fmod_static

# _COCOS_LIB_ANDROID_BEGIN
# _COCOS_LIB_ANDROID_END

include $(BUILD_SHARED_LIBRARY)

$(call import-module,scripting/lua-bindings/proj.android)
$(call import-module,lua-modules)
$(call import-module,fmod/prebuilt/android)

# _COCOS_LIB_IMPORT_ANDROID_BEGIN
# _COCOS_LIB_IMPORT_ANDROID_END
