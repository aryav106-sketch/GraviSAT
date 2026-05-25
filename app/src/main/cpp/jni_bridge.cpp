#include <jni.h>
#include <string>

#include "gravisat_core.h"

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_gravisat_MainActivity_solveSAT(
    JNIEnv* env,
    jobject /* this */,
    jstring inputText
) {

    if (inputText == nullptr) {

        return env->NewStringUTF(
            "NULL INPUT");
    }

    const char* rawText =
        env->GetStringUTFChars(
            inputText,
            nullptr);

    if (rawText == nullptr) {

        return env->NewStringUTF(
            "JNI STRING ERROR");
    }

    std::string cnfText(
        rawText);

    env->ReleaseStringUTFChars(
        inputText,
        rawText);

    std::string result;

    try {

        result =
            solveCNF(
                cnfText);
    }
    catch (
        const std::exception& e) {

        result =
            "NATIVE EXCEPTION: ";

        result += e.what();
    }
    catch (...) {

        result =
            "UNKNOWN NATIVE ERROR";
    }

    return env->NewStringUTF(
        result.c_str());
}
