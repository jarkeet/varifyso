//
// Created by wang on 2017/3/13.
//

#ifndef VARIFYSO_VERIFY_H_H
#define VARIFYSO_VERIFY_H_H
#include <jni.h>
#include <string>
#include "log.h"
using namespace std;
void ByteToHexStr(const char *source, char *dest, int sourceLen);
jstring ToMd5(JNIEnv *env, jbyteArray source);
#endif //VARIFYSO_VERIFY_H_H
