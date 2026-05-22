#include <jni.h>
#include <string>

extern "C"
JNIEXPORT jstring JNICALL
Java_com_gravisat_shield_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {

    std::string hello = "GraviSAT v3.1 Native Engine";

    return env->NewStringUTF(hello.c_str());
}
