LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= jp_pioneer_ceam_handler_GpsChipsController.cpp \


LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libutils \
    libnativehelper \
    libandroid_runtime \
    libgpschipsmanager \
    libbinder \
    
LOCAL_C_INCLUDES += frameworks/base/core/jni \
    device/pioneer/frameworks/Sources/GpsChipsServer/libgpschips/include \
    frameworks/base/include/android_runtime \
    frameworks/native/include/ \
    device/pioneer/frameworks/Sources/GpsChipsServer/include/ \
    device/pioneer/frameworks/Sources/Comm/pinclude/ \

LOCAL_MODULE_TAGS:= optional

LOCAL_MODULE:= libGpsChipsController_JNI

include $(BUILD_SHARED_LIBRARY)




include $(CLEAR_VARS)

LOCAL_SRC_FILES:= jp_pioneer_ceam_handler_NPVicsFmSystem.cpp \


LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libutils \
    libnativehelper \
    libandroid_runtime \
    libbinder \
    libNPVicsFMHandler \
    libPSystemService \
    libsysdIF \
    
LOCAL_C_INCLUDES += frameworks/base/core/jni \
    frameworks/base/include/android_runtime \
    frameworks/native/include/ \
    $(shell find device/pioneer/system/sysd/Handler -type d -name 'publicinc') \
    $(shell find device/pioneer/system/sysd/PCommonLib -type d -name 'publicinc') \
    device/pioneer/system/sysd/PSystemService/publicinc \
    device/pioneer/system/sysd/PCommonLib/NStandardLib/src/ \
    device/pioneer/system/sysd/PCommonLib/NSystemInfoLib/src/ \
    device/pioneer/system/sysd/sysdIFLib/include/ \
    device/pioneer/frameworks/Sources/Comm/pinclude/ \

LOCAL_MODULE_TAGS:= optional

LOCAL_MODULE:= libNPVicsSystem_JNI

include $(BUILD_SHARED_LIBRARY)



include $(CLEAR_VARS)

LOCAL_SRC_FILES:= jp_pioneer_ceam_handler_NPDsrcSystem.cpp \


LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libutils \
	libnativehelper \
	libandroid_runtime \
	libbinder \
	libPCommonLib \
	libsysdIF \
	libNPDsrcHandler \
	libPSystemService \

LOCAL_C_INCLUDES += frameworks/base/core/jni \
		    frameworks/native/include/ \
		    device/pioneer/system/sysd/PCommonLib/NStandardLib/src/ \
		    device/pioneer/system/sysd/PCommonLib/NSystemInfoLib/src/ \
		    $(shell find device/pioneer/system/sysd/Handler -type d -name 'publicinc') \
		    $(shell find device/pioneer/system/sysd/PCommonLib -type d -name 'publicinc') \
		    device/pioneer/system/sysd/sysdIFLib/include/ \
		    device/pioneer/system/sysd/PSystemService/publicinc \
		    #frameworks/base/include/android_runtime \

LOCAL_MODULE_TAGS:= optional

LOCAL_MODULE:= libNPDsrcSystem_JNI

include $(BUILD_SHARED_LIBRARY)

