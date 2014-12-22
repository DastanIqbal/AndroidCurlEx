LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE	:= libcurl-prebuilt
LOCAL_SRC_FILES	:= curl-7.39.0/lib/libcurl.a
LOCAL_EXPORT_C_INCLUDES:=$(LOCAL_PATH)/curl-7.39.0/include/
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := CurlAndroidEx
LOCAL_SRC_FILES := CurlAndroidEx.cpp
LOCAL_STATIC_LIBRARIES:= libcurl-prebuilt
LOCAL_C_INCLUDES:=$(LOCAL_PATH)/curl-7.39.0/include/
LOCAL_LDLIBS	:= -llog -lz
include $(BUILD_SHARED_LIBRARY)

