cmake_minimum_required(VERSION 3.4.1)
#set(jnilibs "${CMAKE_SOURCE_DIR}/src/main/jniLibs")
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${jnilibs}/${ANDROID_ABI})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

add_library( # Sets the name of the library.
             video_player

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             src/main/cpp/video_player.cpp )
target_link_libraries(
                      video_player
                      ${log-lib})


#加载第三方libpng库作为动态库引用
add_library(
          #libxxx.so的名称xxx
          png
          #SHARED 为引入动态库so,STATIC 为引入静态库.a
          SHARED
          IMPORTED
          )

include_directories(
                    #${CMAKE_SOURCE_DIR}/src/main/cpp/librenderer
                    ${CMAKE_SOURCE_DIR}/src/main/cpp/libpng
                    )

SET(
    third_path
    ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libpng.so
    )

MESSAGE(STATUS “src  third so path= ${third_path}”)

#设置要连接的so的相对路径，${ANDROID_ABI}表示so文件的ABI类型的路径,这一步引入了动态加入编译的so
set_target_properties(
                        png
                        PROPERTIES IMPORTED_LOCATION
                        ${third_path}
                     )

#如果当前系统版本过低设置拒绝编译
if (${ANDROID_PLATFORM_LEVEL} LESS 11)
  message(FATAL_ERROR "OpenGL 2 is not supported before API level 11 (currently using ${ANDROID_PLATFORM_LEVEL}).")
  return()
elseif (${ANDROID_PLATFORM_LEVEL} LESS 18)
  add_definitions("-DDYNAMIC_ES3")
  set(OPENGL_LIB GLESv2)
else ()
  set(OPENGL_LIB GLESv3)
endif (${ANDROID_PLATFORM_LEVEL} LESS 11)

MESSAGE(STATUS “opengl lib = ${OPENGL_LIB}”)


add_library( # Sets the name of the library.
             renderer

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             src/main/cpp/librenderer/egl_core.cpp
             src/main/cpp/librenderer/rgba_frame.cpp
             src/main/cpp/librenderer/pic_preview_texture.cpp
             src/main/cpp/librenderer/pic_preview_render.cpp
             src/main/cpp/librenderer/pic_preview_controller.cpp
             src/main/cpp/librenderer/image.c
             src/main/cpp/librenderer/png_decoder.cpp
             src/main/cpp/PngPreviewController.cpp
             )

# Include libraries needed for renderer lib
target_link_libraries(
                      renderer
                      png
                      ${log-lib}
                      ${OPENGL_LIB}
                      android
                      EGL)
