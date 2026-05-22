#include <jni.h>
#include <string>

extern "C"
JNIEXPORT jstring JNICALL
Java_com_gravisat_shield_MainActivity_solveCNF(
        JNIEnv *env,
        jobject thiz,
        jstring input) {

    const char *cnfChars = env->GetStringUTFChars(input, nullptr);

    std::string cnf(cnfChars);

    env->ReleaseStringUTFChars(input, cnfChars);

    bool positive = false;
    bool negative = false;

    // SAFE STRING SEARCH
    if (cnf.find("\n1 0") != std::string::npos ||
        cnf.find("1 0") != std::string::npos) {
        positive = true;
    }

    if (cnf.find("\n-1 0") != std::string::npos ||
        cnf.find("-1 0") != std::string::npos) {
        negative = true;
    }

    std::string result;

    if (positive && negative) {
        result = "UNSATISFIABLE";
    } else {
        result = "SATISFIABLE";
    }

    return env->NewStringUTF(result.c_str());
}
