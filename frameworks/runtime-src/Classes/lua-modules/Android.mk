
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := lua_modules_static
LOCAL_MODULE_FILENAME := libluamodules

LOCAL_SRC_FILES += $(LOCAL_PATH)/lpack/lpack.c

LOCAL_SRC_FILES += $(LOCAL_PATH)/lpeg/lpcap.c \
                    $(LOCAL_PATH)/lpeg/lpcode.c \
                    $(LOCAL_PATH)/lpeg/lpprint.c \
                    $(LOCAL_PATH)/lpeg/lptree.c \
                    $(LOCAL_PATH)/lpeg/lpvm.c

LOCAL_SRC_FILES += $(LOCAL_PATH)/pbc/src/alloc.c \
                   $(LOCAL_PATH)/pbc/src/array.c \
                   $(LOCAL_PATH)/pbc/src/bootstrap.c \
                   $(LOCAL_PATH)/pbc/src/context.c \
                   $(LOCAL_PATH)/pbc/src/decode.c \
                   $(LOCAL_PATH)/pbc/src/map.c \
                   $(LOCAL_PATH)/pbc/src/pattern.c \
                   $(LOCAL_PATH)/pbc/src/proto.c \
                   $(LOCAL_PATH)/pbc/src/register.c \
                   $(LOCAL_PATH)/pbc/src/rmessage.c \
                   $(LOCAL_PATH)/pbc/src/stringpool.c \
                   $(LOCAL_PATH)/pbc/src/varint.c \
                   $(LOCAL_PATH)/pbc/src/wmessage.c \
                   $(LOCAL_PATH)/pbc/binding/lua/pbc-lua.c

LOCAL_SRC_FILES += $(LOCAL_PATH)/lua_modules.cpp

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
                    $(LOCAL_PATH)/../../../cocos2d-x \
                    $(LOCAL_PATH)/../../../cocos2d-x/cocos \
                    $(LOCAL_PATH)/../../../cocos2d-x/cocos/editor-support \
                    $(LOCAL_PATH)/../../../cocos2d-x/cocos/platform/android \
                    $(LOCAL_PATH)/../../../cocos2d-x/cocos/scripting/lua-bindings/manual \
                    $(LOCAL_PATH)/../../../cocos2d-x/external \
                    $(LOCAL_PATH)/../../../cocos2d-x/external/curl/include/android \
                    $(LOCAL_PATH)/../../../cocos2d-x/external/freetype2/include/android/freetype2 \
                    $(LOCAL_PATH)/../../../cocos2d-x/external/lua/luajit/include \
                    $(LOCAL_PATH)/../../../cocos2d-x/external/lua/tolua \
                    $(LOCAL_PATH)/lpeg \
                    $(LOCAL_PATH)/pbc

include $(BUILD_STATIC_LIBRARY)
