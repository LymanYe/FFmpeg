#include <jni.h>
#include <string>
#include "libpng/png.h"

extern "C"
JNIEXPORT jstring

JNICALL
Java_com_lyman_ffmpeg_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {

    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
