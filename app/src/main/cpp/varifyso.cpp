//
// Created by Jeff on 2017/3/10.
//


#include <jni.h>
#include <string>
#include "log.h"
using namespace std;

#define RELEASE_MODE 1


const char *ClassName = "com/cmcc/jarkeet/varifyso/VerifySign";//指定要注册的类 即nativie方法所在的类


static jstring jniGetPassword(JNIEnv *env, jobject obj){

    return env->NewStringUTF("password from jni");

}

void ByteToHexStr(const char *source, char *dest, int sourceLen) {
    short i;
    char highByte, lowByte;

    for (i = 0; i < sourceLen; i++) {
        highByte = source[i] >> 4;
        lowByte = source[i] & 0x0f;
        highByte += 0x30;

        if (highByte > 0x39) {
            dest[i * 2] = highByte + 0x07;
        } else {
            dest[i * 2] = highByte;
        }

        lowByte += 0x30;
        if (lowByte > 0x39) {
            dest[i * 2 + 1] = lowByte + 0x07;
        } else {
            dest[i * 2 + 1] = lowByte;
        }
    }
}


jstring ToMd5(JNIEnv *env, jbyteArray source) {
    // MessageDigest类
    jclass classMessageDigest = env->FindClass("java/security/MessageDigest");
    // MessageDigest.getInstance()静态方法
    jmethodID midGetInstance = env->GetStaticMethodID(classMessageDigest, "getInstance", "(Ljava/lang/String;)Ljava/security/MessageDigest;");
    // MessageDigest object
    jobject objMessageDigest = env->CallStaticObjectMethod(classMessageDigest, midGetInstance, env->NewStringUTF("md5"));

    // update方法，这个函数的返回值是void，写V
    jmethodID midUpdate = env->GetMethodID(classMessageDigest, "update", "([B)V");
    env->CallVoidMethod(objMessageDigest, midUpdate, source);

    // digest方法
    jmethodID midDigest = env->GetMethodID(classMessageDigest, "digest", "()[B");
    jbyteArray objArraySign = (jbyteArray) env->CallObjectMethod(objMessageDigest, midDigest);

    jsize intArrayLength = env->GetArrayLength(objArraySign);
    jbyte* byte_array_elements = env->GetByteArrayElements(objArraySign, NULL);
    size_t length = (size_t) intArrayLength * 2 + 1;
    char* char_result = (char*) malloc(length);
    memset(char_result, 0, length);

    // 将byte数组转换成16进制字符串，发现这里不用强转，jbyte和unsigned char应该字节数是一样的
    ByteToHexStr((const char*)byte_array_elements, char_result, intArrayLength);
    // 在末尾补\0
    *(char_result + intArrayLength * 2) = '\0';
    LOGD("print sign from jni %s", char_result);
    jstring stringResult = env->NewStringUTF(char_result);
    // release
    env->ReleaseByteArrayElements(objArraySign, byte_array_elements, JNI_ABORT);
    // 释放指针使用free
    free(char_result);
    return stringResult;
}

jstring jniGetSignature(JNIEnv *env, jobject object, jobject contextObject) {
    jclass contextClass = env->FindClass("android/content/Context");//获取Context class
    jclass signatureClass = env->FindClass("android/content/pm/Signature");//获取Signature class
    jclass packageNameClass = env->FindClass("android/content/pm/PackageManager");// get PackageManager class
    jclass packageInfoClass = env->FindClass("android/content/pm/PackageInfo");// get PackageInfo class

    jmethodID getPackageManagerId = env->GetMethodID(contextClass, "getPackageManager",
                                                     "()Landroid/content/pm/PackageManager;");// getPackageManager jmethodId
    jmethodID getPackageNameId = env->GetMethodID(contextClass, "getPackageName",
                                                  "()Ljava/lang/String;");
    jmethodID getPackageInfoId = env->GetMethodID(packageNameClass, "getPackageInfo",    //getPackageInfo jmethodId
                                                  "(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;");


    jobject packageManagerObject = env->CallObjectMethod(contextObject, getPackageManagerId);
    jstring packNameString = (jstring) env->CallObjectMethod(contextObject, getPackageNameId);
    jobject packageInfoObject = env->CallObjectMethod(packageManagerObject, getPackageInfoId,
                                                      packNameString, 64);
    jfieldID signaturefieldID = env->GetFieldID(packageInfoClass, "signatures",
                                                "[Landroid/content/pm/Signature;");
    jobjectArray signatureArray = (jobjectArray) env->GetObjectField(packageInfoObject,
                                                                     signaturefieldID);
    jobject signatureObject = env->GetObjectArrayElement(signatureArray, 0);

    // 得到toCharsString方法的ID
    jmethodID getByteArray = env->GetMethodID(signatureClass, "toByteArray", "()[B");
    // 返回当前应用签名信息
    jbyteArray signatureByteArray = (jbyteArray) env->CallObjectMethod(signatureObject, getByteArray);

    return ToMd5(env, signatureByteArray);

}


/****
 * 声明需要动态注册的方法
 */
static JNINativeMethod gMethods[] = {
        {"getNativeSignature",     "(Landroid/content/Context;)Ljava/lang/String;", (void *) jniGetSignature}
        ,{"getNativePassword",     "()Ljava/lang/String;",     (void *) jniGetPassword}
};


/****
 * 注册方法
 */
static int registerNativeMethods(JNIEnv *env, const char *className, JNINativeMethod *gMethods, int numMethods) {
    jclass clazz;
    clazz = env->FindClass(className);
    if (clazz == NULL) {
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

static int registerNatives(JNIEnv *env) {
    return registerNativeMethods(env, ClassName, gMethods, sizeof(gMethods)/sizeof(JNINativeMethod));
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv *env;
    if (vm->GetEnv((void **) (&env), JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }

    LOGI("Library JNI_OnLoad begin =========");

    if(registerNatives(env) < 0) {
        LOGE("registerNatives error.");
        return -1;
    }

    // RELEASE_MODE这个宏是通过编译脚本设定的，如果是release模式，
    // 则RELEASE_MODE=1，否则为0或者未定义
#ifdef RELEASE_MODE
//    if (RELEASE_MODE == 1) {
//        // 检查当前应用的签名是否一致，如果不签名不一致的话，则直接退出
//        if (checkSignature(env) != JNI_TRUE) {
//            LOGE("The app signature is NOT correct, please check the apk signture. ");
//            LOGI("Library JNI_OnLoad end ===========");
//            return -1;
//        } else {
//            LOGI("    The app signature is correct.");
//        }
//    } else {
//        // Do nothing
//    }
#endif

    LOGI("Library JNI_OnLoad end ===========");
    return JNI_VERSION_1_6;
}

