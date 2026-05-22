#include <jni.h>
#include <string>

#include "solver.h"

extern "C"
JNIEXPORT jstring JNICALL
Java_com_gravisat_shield_MainActivity_solveCNF(
        JNIEnv *env,
        jobject,
        jstring input) {

    const char *rawText =
            env->GetStringUTFChars(input, nullptr);

    std::string cnf(rawText);

    env->ReleaseStringUTFChars(input, rawText);

    Solver solver;

    bool sat = solver.solve(cnf);

    std::string output;

    if (sat) {

        output = "SATISFIABLE\n\n";

        output += solver.getSolution();

    } else {

        output = "UNSATISFIABLE";
    }

    return env->NewStringUTF(output.c_str());
}
