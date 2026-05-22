#include <jni.h>
#include <string>
#include "solver.h"

extern "C"
JNIEXPORT jstring JNICALL
Java_com_gravisat_shield_MainActivity_solveSAT(
        JNIEnv *env,
        jobject /* this */,
        jstring input) {

    if (input == nullptr) {
        return env->NewStringUTF("NULL INPUT");
    }

    const char *rawInput = env->GetStringUTFChars(input, nullptr);

    if (rawInput == nullptr) {
        return env->NewStringUTF("JNI ERROR");
    }

    std::string cnf(rawInput);

    env->ReleaseStringUTFChars(input, rawInput);

    Solver solver;

    bool result = solver.solve(cnf);

    std::string output =
            result ? "SATISFIABLE" : "UNSATISFIABLE";

    return env->NewStringUTF(output.c_str());
}
