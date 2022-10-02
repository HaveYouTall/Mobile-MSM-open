#include <jni.h>
#include <string>
#include <iostream>
#include <fstream>

jstring charTojstring(JNIEnv* env, const char* pat) {
    jclass strClass = (env)->FindClass("java/lang/String");
    jmethodID ctorID = (env)->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    jbyteArray bytes = (env)->NewByteArray(strlen(pat));
    (env)->SetByteArrayRegion(bytes, 0, strlen(pat), (jbyte*) pat);
    jstring encoding = (env)->NewStringUTF("GB2312");
    return (jstring) (env)->NewObject(strClass, ctorID, bytes, encoding);
}


 char* jstringToChar(JNIEnv* env, jstring jstr) {
     char* rtn = NULL;
     jclass clsstring = env->FindClass("java/lang/String");
     jstring strencode = env->NewStringUTF("GB2312");
     jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
     jbyteArray barr = (jbyteArray) env->CallObjectMethod(jstr, mid, strencode);
     jsize alen = env->GetArrayLength(barr);
     jbyte* ba = env->GetByteArrayElements(barr, JNI_FALSE);
     if (alen > 0) {
         rtn = (char*) malloc(alen + 1);
         memcpy(rtn, ba, alen);
         rtn[alen] = 0;
     }
     env->ReleaseByteArrayElements(barr, ba, 0);
     return rtn;
 }

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_mobilemsm_YYteamMSM_readResultTime(JNIEnv *env, jclass clazz, jstring dir) {
    char* chardata = jstringToChar(env, dir);
    std::string absDir = chardata;

    std::ifstream fin(absDir + "/resulttime.txt", std::ios::in);
    if(!fin.is_open()){
        std::cout << "Error open filep";
        return charTojstring(env, "fail open file");
    }
    std::string resTime;
    std::string retValue = "";
    while (std::getline(fin, resTime)) {
        if(resTime.find("Total ave time:") == std::string::npos) {
            continue;
        }
        retValue = resTime.substr(resTime.find(": ") + 1) + "\n";
    }
    fin.close();

    return charTojstring(env, (char*)retValue.c_str());

}