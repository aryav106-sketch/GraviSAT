#include <jni.h>
#include <string>

#include "solver.h"

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_gravisat_MainActivity_solveSAT(
        JNIEnv* env,
        jobject /* this */) {

    GraviSATCore solver;

    solver.solve();

    std::string result = solver.getResult();

    return env->NewStringUTF(result.c_str());
}
