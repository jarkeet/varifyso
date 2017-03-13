//
// Created by Jeff on 2017/3/10.
//
#include "verify.h"
//#define RELEASE_MODE 0

const char *VERIFYSIGN_CLASS_NAME = "com/cmcc/jarkeet/varifyso/VerifySign";//指定要注册的类 即nativie方法所在的类

const char *JCONTEXT_CLASS_NAME = "com/cmcc/jarkeet/varifyso/NativeContext";//application class name
const char *METHOD_NAME_GETAPPCONTEXT = "getAppContext";// java method get app context
const char *METHOD_SIGNATURE_GETAPPCONTEXT = "()Landroid/content/Context;";//  method signature get app context

const char *APP_SIGNATURE = "E73A01DF5CC975430E44224A6F75C280";

/**
 * get App Signature by native
 * @param env
 * @param object
 * @param contextObject
 * @return
 */
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

/**
 * check App Signature
 * @param env
 * @return
 */
static jboolean checkSignature(JNIEnv *env) {
    //得到当前app的NativeContext
    jclass classNativeContext = env->FindClass(JCONTEXT_CLASS_NAME);
    //得到getAppContext静态方法
    jmethodID midGetAppContext = env->GetStaticMethodID(classNativeContext,
                                                        METHOD_NAME_GETAPPCONTEXT,
                                                        METHOD_SIGNATURE_GETAPPCONTEXT);
    //调用getAppContext方法得到conext对象
    jobject appContext = env->CallStaticObjectMethod(classNativeContext, midGetAppContext);
    jboolean result = JNI_FALSE;
    if(appContext != NULL) {
        jstring appSignature = jniGetSignature(env, NULL, appContext);
        jstring releaseSignature = env->NewStringUTF(APP_SIGNATURE);
        const char* charAppSignature = env->GetStringUTFChars(appSignature, NULL);
        const char* charReleaseSignature = env->GetStringUTFChars(releaseSignature, NULL);
        if(charAppSignature != NULL && charReleaseSignature != NULL) {
            if(strcmp(charAppSignature, charReleaseSignature) == 0) {
                result = JNI_TRUE;
            }
        }
        env->ReleaseStringUTFChars(appSignature, charAppSignature);
        env->ReleaseStringUTFChars(releaseSignature, charReleaseSignature);

    }
    if(result == JNI_TRUE) {
        LOGI("jni checkSignature true");
    } else {
        LOGE("jni checkSignature --> failed");
    }
    return result;
}

static jstring jniGetPassword(JNIEnv *env, jobject obj){

    jboolean authn = checkSignature(env);
    if(authn) {
        return env->NewStringUTF("password from jni : 123456");
    } else {
        return env->NewStringUTF("verify so error...no permission .");
    }
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
    return registerNativeMethods(env, VERIFYSIGN_CLASS_NAME, gMethods, sizeof(gMethods)/sizeof(JNINativeMethod));
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
    if (RELEASE_MODE == 1) {
        // 检查当前应用的签名是否一致，如果不签名不一致的话，则直接退出
        if (checkSignature(env) != JNI_TRUE) {
            LOGE("The release version app signature is NOT correct, please check the apk signture. ");
            LOGI("Library JNI_OnLoad end ===========");
            return -1;
        } else {
            LOGI("The app signature is correct.");
        }
    } else {
        // Do nothing
    }
#endif

    LOGI("Library JNI_OnLoad end ===========");
    return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved) {
    LOGI("Library JNI_OnUnload begin =========");
    LOGI("Library JNI_OnUnload end ===========");
}

