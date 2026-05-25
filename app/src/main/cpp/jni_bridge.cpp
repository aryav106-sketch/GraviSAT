#include <jni.h>
#include <string>

#include "gravisat_core.h"

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_gravisat_MainActivity_solveSAT(
        JNIEnv* env,
        jobject thiz) {

    GraviSATCore solver;

    std::string result = solver.getResult();

    return env->NewStringUTF(result.c_str());
}
