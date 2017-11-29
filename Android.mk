LOCAL_PATH:= $(call my-dir)

# libusbdemon

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \



#for plist 
LOCAL_SRC_FILES += \
                   src/libplist/Array.cpp \
                   src/libplist/base64.c \
                   src/libplist/Boolean.cpp \
                   src/libplist/bplist.c \
                   src/libplist/bytearray.c \
                   src/libplist/Data.cpp \
                   src/libplist/Date.cpp \
                   src/libplist/Dictionary.cpp \
                   src/libplist/hashtable.c \
                   src/libplist/Integer.cpp \
                   src/libplist/Key.cpp \
                   src/libplist/Node.cpp \
                   src/libplist/plist.c \
                   src/libplist/ptrarray.c \
                   src/libplist/Real.cpp \
                   src/libplist/String.cpp \
                   src/libplist/Structure.cpp \
                   src/libplist/time64.c \
                   src/libplist/Uid.cpp \
                   src/libplist/xplist.c \
                   src/libplist/cnary.c \
                   src/libplist/iterator.c \
                   src/libplist/list.c \
                   src/libplist/node.c \
                   src/libplist/node_iterator.c \
                   src/libplist/node_list.c 

LOCAL_C_INCLUDES += \
  $(LOCAL_PATH)/include \
  $(LOCAL_PATH)/include/plist \
  $(LOCAL_PATH)/src/libplist \



LOCAL_SRC_FILES += \
  src/collection.c \
  src/socket.c \
  src/libusbmuxd.c \
  src/libjni.c



include external/stlport/libstlport.mk



LOCAL_SHARED_LIBRARIES += \
	libcutils \
	libutils \


LOCAL_LDLIBS := -llog

LOCAL_CFLAGS += -pie -fPIE

LOCAL_MODULE := libusbsocket

include $(BUILD_SHARED_LIBRARY)

#LOCAL_MODULE := libusbsocket
#LOCAL_SRC_FILES +=  src/maintest.c
#include $(BUILD_EXECUTABLE)
