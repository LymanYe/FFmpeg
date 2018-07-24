cmake_minimum_required(VERSION 3.4.1)
find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

set(PATH_TO_ROOT ${CMAKE_SOURCE_DIR}/src/main/cpp)

include_directories(${PATH_TO_ROOT}/libcommon/)

file(GLOB FILES_LIB_COMMON "${PATH_TO_ROOT}/libcommon/*.cpp")
file(GLOB FILES_LIB_COMMON_EGL_CORE "${PATH_TO_ROOT}/libcommon/egl_core/*.cpp")
file(GLOB FILES_LIB_COMMON_MSG_Q "${PATH_TO_ROOT}/libcommon/message_queue/*.cpp")
file(GLOB FILES_LIB_COMMON_GL_MEDIA "${PATH_TO_ROOT}/libcommon/opengl_media/*.cpp")
file(GLOB FILES_LIB_COMMON_GL_MEDIA_RENDER "${PATH_TO_ROOT}/libcommon/opengl_media/render/*.cpp")
file(GLOB FILES_LIB_COMMON_GL_MEDIA_TEXTURE "${PATH_TO_ROOT}/libcommon/opengl_media/texture/*.cpp")
file(GLOB FILES_LIB_COMMON_GL_MEDIA_TEXTURE_COPIER "${PATH_TO_ROOT}/libcommon/opengl_media/texture_copier/*.cpp")
file(GLOB FILES_LIB_COMMON_SL_MEDIA "${PATH_TO_ROOT}/libcommon/opensl_media/*.cpp")

add_library( # Sets the name of the library.
             commontool

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             ${FILES_LIB_COMMON}
             ${FILES_LIB_COMMON_EGL_CORE}
             ${FILES_LIB_COMMON_MSG_Q}
             ${FILES_LIB_COMMON_GL_MEDIA}
             ${FILES_LIB_COMMON_GL_MEDIA_RENDER}
             ${FILES_LIB_COMMON_GL_MEDIA_TEXTURE}
             ${FILES_LIB_COMMON_GL_MEDIA_TEXTURE_COPIER}
             ${FILES_LIB_COMMON_SL_MEDIA}
             )

# Include libraries needed for renderer lib
target_link_libraries(
                      commontool
                      ${log-lib}
                      android
                      GLESv2
                      EGL
                      OpenSLES)

include_directories(${PATH_TO_ROOT}/3rdparty/ffmpeg/include)
add_library(
              libffmpeg
              SHARED
              IMPORTED
)

set_target_properties(libffmpeg
                      PROPERTIES IMPORTED_LOCATION
                      ${CMAKE_SOURCE_DIR}/src/main/jniLibs/armeabi-v7a/libffmpeg.so
)

file(GLOB FILES_LIB_VIDEO_ENCODER "${PATH_TO_ROOT}/video_encoder/*.cpp")
file(GLOB FILES_LIB_VIDEO_ENCODER_HW "${PATH_TO_ROOT}/video_encoder/hw_encoder/*.cpp")
file(GLOB FILES_LIB_VIDEO_ENCODER_SOFT "${PATH_TO_ROOT}/video_encoder/soft_encoder/*.cpp")
file(GLOB FILES_LIB_VIDEO_ENCODER_SOFT_COLOR "${PATH_TO_ROOT}/video_encoder/soft_encoder/color_conversion/*.cpp")

add_library( # Sets the name of the library.
             videoencoder

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             ${FILES_LIB_VIDEO_ENCODER}
             ${FILES_LIB_VIDEO_ENCODER_HW}
             ${FILES_LIB_VIDEO_ENCODER_SOFT}
             ${FILES_LIB_VIDEO_ENCODER_SOFT_COLOR}
             )

target_link_libraries(
                      videoencoder
                      commontool
                      libffmpeg)

include_directories(${PATH_TO_ROOT}/camera_preview/)

file(GLOB FILES_LIB_CAMERA_PREVIEW "${PATH_TO_ROOT}/camera_preview/*.cpp")

add_library( # Sets the name of the library.
             camerapreview

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             ${FILES_LIB_CAMERA_PREVIEW}
             ${PATH_TO_ROOT}/LPreviewScheduler.cpp
             )

# Include libraries needed for renderer lib
target_link_libraries(
                      camerapreview
                      videoencoder)

