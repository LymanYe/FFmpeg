cmake_minimum_required(VERSION 3.4.1)
#set(jnilibs "${CMAKE_SOURCE_DIR}/src/main/jniLibs")
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${jnilibs}/${ANDROID_ABI})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall")

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

add_library( # Sets the name of the library.
             audio_player

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             src/main/cpp/audio_player.cpp )

# Include libraries needed for native-audio-jni lib
target_link_libraries(
                      audio_player
                      #native-audio-jni
                      android
                      log
                      OpenSLES
                      )
