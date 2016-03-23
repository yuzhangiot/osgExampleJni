LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := osgNativeLib
### Main Install dir
OSG_ANDROID_DIR	:= /home/joseph/osg/osg-android/build
LIBDIR 			:= $(OSG_ANDROID_DIR)/obj/local/armeabi
ANDROID_NDK		:= /home/joseph/android/android-ndk-r9d
OSG_3RDPARTY_DIR := /home/joseph/osg/osg-android/3rdparty
OSG_3RDPARTY_COMPONENTS := jpeg png libavcodec libavdevice libavfilter libavformat libavutil libswresample libswscale

ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
	LOCAL_ARM_NEON 	:= true
	LIBDIR 			:= $(OSG_ANDROID_DIR)/obj/local/armeabi-v7a
endif

### Add all source file names to be included in lib separated by a whitespace

LOCAL_C_INCLUDES:= $(OSG_ANDROID_DIR)/include /home/joseph/osg/osg-android/include /home/joseph/osg/osg-android/3rdparty/libpng
LOCAL_CFLAGS    := -Werror -fno-short-enums
LOCAL_CPPFLAGS  := -DOSG_LIBRARY_STATIC 

 LOCAL_WHOLE_STATIC_LIBRARIES :=$(OSG_3RDPARTY_COMPONENTS)

LOCAL_LDLIBS    := -llog -lGLESv2 -lz -Istdc++ -lm -ldl -lgcc
LOCAL_LDLIBS	 += -L$(ANDROID_NDK)/sources/cxx-stl/gnu-libstdc++/4.6/libs/armeabi -lgnustl_static
LOCAL_LDLIBS 	 += -L $(LIBDIR) \
-losg \
-lavformat \
-lavcodec \
-lavdevice \
-lavfilter \
-lavutil \
-lswscale \
-lswresample
LOCAL_SRC_FILES := wcmapp3osgNativeLib.cpp OsgMainApp.cpp OsgAndroidNotifyHandler.cpp OsgMovieEventHandler.cpp
LOCAL_LDFLAGS   := -L $(LIBDIR) \
-L$(OSG_3RDPARTY_DIR) \
-losgdb_dds \
-losgdb_openflight \
-losgdb_tga \
-losgdb_rgb \
-losgdb_png \
-losgdb_jpeg \
-losgdb_osgterrain \
-losgdb_osg \
-losgdb_ive \
-losgdb_deprecated_osgviewer \
-losgdb_deprecated_osgvolume \
-losgdb_deprecated_osgtext \
-losgdb_deprecated_osgterrain \
-losgdb_deprecated_osgsim \
-losgdb_deprecated_osgshadow \
-losgdb_deprecated_osgparticle \
-losgdb_deprecated_osgfx \
-losgdb_deprecated_osganimation \
-losgdb_deprecated_osg \
-losgdb_serializers_osgvolume \
-losgdb_serializers_osgtext \
-losgdb_serializers_osgterrain \
-losgdb_serializers_osgsim \
-losgdb_serializers_osgshadow \
-losgdb_serializers_osgparticle \
-losgdb_serializers_osgmanipulator \
-losgdb_serializers_osgfx \
-losgdb_serializers_osganimation \
-losgdb_serializers_osg \
-losgViewer \
-losgVolume \
-losgTerrain \
-losgText \
-losgShadow \
-losgSim \
-losgParticle \
-losgManipulator \
-losgGA \
-losgFX \
-losgDB \
-losgAnimation \
-losgUtil \
-losg \
-lOpenThreads \
-ljpeg \
-lpng \
-lavformat \
-lavcodec \
-lavdevice \
-lavfilter \
-lavutil \
-lswscale \
-lswresample \
-losgdb_ffmpeg

include $(BUILD_SHARED_LIBRARY)
