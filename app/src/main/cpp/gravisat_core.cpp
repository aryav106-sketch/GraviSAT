#include <android/log.h>

#define LOG_TAG "GraviSAT"

void gravisat_init() {

    __android_log_print(
            ANDROID_LOG_INFO,
            LOG_TAG,
            "GraviSAT v3.0 Initialized"
    );
}
