#include "com_example_lesah_000_ndktest_MainActivity.h"

/*
 * Class:     com_example_lesah_000_ndktest_MainActivity
 * Method:    HelloJNI
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_example_lesah_1000_ndktest_MainActivity_HelloJNI
  (JNIEnv *env, jobject obj){
       (*env)->NewStringUTF(env, "Hello From JNI");
  }