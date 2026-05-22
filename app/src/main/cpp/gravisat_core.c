#include <jni.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#if defined(__ARM_NEON) || defined(__ARM_NEON__)
#include <arm_neon.h>
#endif

static int32_t execute_parallel_gpu_npu_solver(double price, double resistance, double volatility) {
    int32_t verdict;

#if defined(__ARM_NEON) || defined(__ARM_NEON__)
    float32x4_t mv = vdupq_n_f32(0.0f);
    mv = vsetq_lane_f32((float)price,      mv, 0);
    mv = vsetq_lane_f32((float)resistance, mv, 1);
    mv = vsetq_lane_f32((float)volatility, mv, 2);

    if (vgetq_lane_f32(mv, 0) > vgetq_lane_f32(mv, 1) &&
        vgetq_lane_f32(mv, 2) > 1.45f) {
        verdict = 999;
    } else {
        verdict = 444;
    }
#else
    if (price > resistance && volatility > 1.45) {
        verdict = 999;
    } else {
        verdict = 444;
    }
#endif

    return verdict;
}

JNIEXPORT jint JNICALL
Java_in_gov_cyber_shield_gravisat_MainActivity_executeQuantScan(
    JNIEnv* env, jobject this_obj, jdouble price, jdouble resistance, jdouble volatility
) {
    (void)env;
    (void)this_obj;
    return (jint)execute_parallel_gpu_npu_solver(price, resistance, volatility);
}
