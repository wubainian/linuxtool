LOCAL_PATH := $(call my-dir)

###############################################################
include $(CLEAR_VARS)
LOCAL_MODULE := pagemap

LOCAL_CFLAGS := -Wno-error=format-security -fpermissive
LOCAL_CFLAGS += -fno-rtti -fno-exceptions

LOCAL_SRC_FILES := pagemap.c

LOCAL_LDLIBS := -llog

include $(BUILD_EXECUTABLE)

###############################################################

include $(CLEAR_VARS)
LOCAL_MODULE := pagemap2

LOCAL_CFLAGS := -Wno-error=format-security -fpermissive
LOCAL_CFLAGS += -fno-rtti -fno-exceptions

LOCAL_SRC_FILES := pagemap2.c

LOCAL_LDLIBS := -llog

include $(BUILD_EXECUTABLE)

###############################################################

include $(CLEAR_VARS)
LOCAL_MODULE := mincore

LOCAL_CFLAGS := -Wno-error=format-security -fpermissive
LOCAL_CFLAGS += -fno-rtti -fno-exceptions

LOCAL_SRC_FILES := pagecache.cpp

LOCAL_LDLIBS := -llog

include $(BUILD_EXECUTABLE)
