cmake_minimum_required(VERSION 3.4.1)
find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

set(PATH_TO_MEDIA_CORE ${CMAKE_SOURCE_DIR}/src/main/cpp)
set(PATH_TO_THIRD_PARTY ${PATH_TO_MEDIA_CORE}/3rdparty/ffmpeg/include/)
set(PATH_TO_PRE_BUILT ${PATH_TO_MEDIA_CORE}/3rdparty/prebuilt/${ANDROID_ABI})

include_directories(${PATH_TO_THIRD_PARTY})
include_directories(${PATH_TO_MEDIA_CORE}/)
include_directories(${PATH_TO_MEDIA_CORE}/common/)
include_directories(${PATH_TO_MEDIA_CORE}/camera_preview/)
include_directories(${PATH_TO_MEDIA_CORE}/video_encoder/)


file(GLOB FILES_CAMERA_PREVIEW "${PATH_TO_MEDIA_CORE}/camera_preview/*.cpp")

file(GLOB FILES_COMMON "${PATH_TO_MEDIA_CORE}/common/*.cpp")
file(GLOB FILES_COMMON_EGL_CORE "${PATH_TO_MEDIA_CORE}/common/egl_core/*.cpp")
file(GLOB FILES_COMMON_MSG_Q "${PATH_TO_MEDIA_CORE}/common/message_queue/*.cpp")
file(GLOB FILES_COMMON_GL_MEDIA "${PATH_TO_MEDIA_CORE}/common/opengl_media/*.cpp")
file(GLOB FILES_COMMON_GL_MEDIA_RENDER "${PATH_TO_MEDIA_CORE}/common/opengl_media/render/*.cpp")
file(GLOB FILES_COMMON_GL_MEDIA_TEXTURE "${PATH_TO_MEDIA_CORE}/common/opengl_media/texture/*.cpp")
file(GLOB FILES_COMMON_GL_MEDIA_TEXTURE_COPIER "${PATH_TO_MEDIA_CORE}/common/opengl_media/texture_copier/*.cpp")
file(GLOB FILES_COMMON_SL_MEDIA "${PATH_TO_MEDIA_CORE}/common/opensl_media/*.cpp")

file(GLOB FILES_VIDEO_ENCODER "${PATH_TO_MEDIA_CORE}/video_encoder/*.cpp")
file(GLOB FILES_VIDEO_ENCODER_HW "${PATH_TO_MEDIA_CORE}/video_encoder/hw_encoder/*.cpp")
file(GLOB FILES_VIDEO_ENCODER_SOFT "${PATH_TO_MEDIA_CORE}/video_encoder/soft_encoder/*.cpp")
file(GLOB FILES_VIDEO_ENCODER_SOFT_COLOR "${PATH_TO_MEDIA_CORE}/video_encoder/soft_encoder/color_conversion/*.cpp")



add_library( # Sets the name of the library.
             camerapreview

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             ${FILES_THIRD_PARTY}
             ${FILES_CAMERA_PREVIEW}
             ${FILES_COMMON}
             ${FILES_COMMON_EGL_CORE}
             ${FILES_COMMON_MSG_Q}
             ${FILES_COMMON_GL_MEDIA}
             ${FILES_COMMON_GL_MEDIA_RENDER}
             ${FILES_COMMON_GL_MEDIA_TEXTURE}
             ${FILES_COMMON_GL_MEDIA_TEXTURE_COPIER}
             ${FILES_COMMON_SL_MEDIA}
             ${FILES_VIDEO_ENCODER}
             ${FILES_VIDEO_ENCODER_HW}
             ${FILES_VIDEO_ENCODER_SOFT}
             ${FILES_VIDEO_ENCODER_SOFT_COLOR}
             ${PATH_TO_MEDIA_CORE}/LPreviewScheduler.cpp
             )

# Include libraries needed for renderer lib
target_link_libraries(
                      camerapreview
                      # 引入系统的动态库
                      log
                      android
                      GLESv2
                      EGL
                      z
                      OpenSLES
                      # 引入ffmpeg相关静态库
                      ${PATH_TO_PRE_BUILT}/libavfilter.a
                      ${PATH_TO_PRE_BUILT}/libavformat.a
                      ${PATH_TO_PRE_BUILT}/libavcodec.a
                      ${PATH_TO_PRE_BUILT}/libpostproc.a
                      ${PATH_TO_PRE_BUILT}/libswresample.a
                      ${PATH_TO_PRE_BUILT}/libswscale.a
                      ${PATH_TO_PRE_BUILT}/libavutil.a
                      ${PATH_TO_PRE_BUILT}/libpostproc.a
                      ${PATH_TO_PRE_BUILT}/libfdk-aac.a
                      ${PATH_TO_PRE_BUILT}/libvo-aacenc.a
                      ${PATH_TO_PRE_BUILT}/libx264.a
                      )

