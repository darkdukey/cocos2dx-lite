LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := cocos_fmod_static
LOCAL_MODULE_FILENAME := fmod
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/libfmod.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../../include/android
include $(PREBUILT_STATIC_LIBRARY)
