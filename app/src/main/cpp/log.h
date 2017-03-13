

#ifndef JNIDEMO_JNIDEBUG_H
#define JNIDEMO_JNIDEBUG_H
#include "jni.h"
#include "stdio.h"
#include <android/log.h>

#ifdef __cplusplus
extern "C" {
#endif

// log标签
#define  LOG_TAG    "jarkeet_JNITAG"
// 定义info信息
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
// 定义debug信息
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
// 定义error信息
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#ifdef __cplusplus
}
#endif


#endif //JNIDEMO_JNIDEBUG_H
