#include <jni.h>
#include <string>
#include <vector>
#include <sstream>

extern "C"
JNIEXPORT jstring JNICALL
Java_com_gravisat_shield_MainActivity_solveCNF(
        JNIEnv* env,
        jobject /* this */,
        jstring input) {

    const char* raw = env->GetStringUTFChars(input, 0);

    std::string cnf(raw);

    env->ReleaseStringUTFChars(input, raw);

    // ===== SIMPLE CHECK =====

    bool hasPos = false;
    bool hasNeg = false;

    std::istringstream iss(cnf);
    std::string line;

    while (std::getline(iss, line)) {

        if (line == "1 0")
            hasPos = true;

        if (line == "-1 0")
            hasNeg = true;
    }

    // ===== RESULT =====

    std::string result;

    if (hasPos && hasNeg) {
        result = "UNSATISFIABLE";
    }
    else {
        result = "SATISFIABLE";
    }

    return env->NewStringUTF(result.c_str());
}
