#include "com_example_lesah_000_ndktest_Valera.h"
#include "cli.h"
#include <stdio.h>
#include <string.h>
//#include "base/android/jni_string.h"
//#include "base/strings/utf_string_conversions.h"

/*
 * Class:     com_example_lesah_000_ndktest_MainActivity
 * Method:    HelloJNI
 * Signature: ()Ljava/lang/String;
 */

#define LOGGING1
#ifdef LOGGING
#define LOG(x, ... ) do{fprintf(log_file, x , ##__VA_ARGS__ );fflush(log_file);}while(0)
#else
#define LOG(x, ... ) do{}while(0)
#endif //LOGGING
int des=0;
int connected = 0;
int rc = 0;
uint32_t Msg;
uint32_t u32Msg;
uint32_t fFormats;
uint32_t client;
FILE* log_file;


int VbClipConnect(){
	if ( connected == 0 )
	{
		connected = 1;
		des = open("/dev/vboxguest", O_RDWR );
		
		#ifdef LOGGING
		log_file = fopen( "/mnt/sdcard/log_lesa", "w");
		//if ( log_des < 1 ) sleep(5);
		LOG("Descriptor = %d\n", des);
		#endif
		return VbglR3ClipboardConnect(&client);
		
	}
}

JNIEXPORT jstring JNICALL Java_com_example_lesah_1000_ndktest_Valera_HelloJNI
  (JNIEnv *env, jobject obj){
	 
	VbClipConnect();
	
	rc = VbglR3ClipboardGetHostMsg(client, &Msg, &fFormats);
	
	switch( Msg)
        {
        case VBOX_SHARED_CLIPBOARD_HOST_MSG_FORMATS:
        {
			LOG("IN VBOX_SHARED_CLIPBOARD_HOST_MSG_FORMATS\n");
            LOG("Msg = MSG_FORMATS\n");
            LOG("fFormats =  %u\n" , fFormats);
			char valera[10000];
            uint32_t count;
			if ( fFormats == 0 ){
				 return (*env)->ThrowNew(env, (*env)->FindClass(env, "java/lang/Exception"), "");
			}
            rc = VbglR3ClipboardReadData(client, fFormats, (void*) valera, 10000, &count);
			LOG("VbglR3ClipboardReadData retval = %d\n", rc);
			
			jstring result = (*env)->NewString(env, (jchar*)valera,count / 2 );
			return result;
            break;
        }
        case VBOX_SHARED_CLIPBOARD_HOST_MSG_READ_DATA:
        {
			LOG("IN VBOX_SHARED_CLIPBOARD_HOST_MSG_READ_DATA\n");
			char valera[] = {0x1f,0x4,0x40,0x4,0x38, 0x4, 0x32, 0x4, 0x35, 0x4, 0x42, 0x4, 0x21, 0, 0, 0};
			jstring result = (*env)->NewString(env, (jchar*)valera,0 );
			return result;
            //printf("Msg = MSG_READ_DATA\n");
			//int VbglR3ClipboardWriteData(uint32_t u32ClientId, uint32_t fFormat, void *pv, uint32_t cb)
			//rc = VbglR3ClipboardWriteData( client, 1 , valera , 7);
            //break;
        }
        case VBOX_SHARED_CLIPBOARD_HOST_MSG_QUIT:
        {
            LOG("Msg = MSG_QUIT\n");
			//return throwOutOfMemoryError( env, "throwNoSuchMethodError: VBOX_SHARED_CLIPBOARD_HOST_MSG_QUIVBOX_SHARED_CLIPBOARD_HOST_MSG_QUIT" );
			return (*env)->ThrowNew(env, (*env)->FindClass(env, "java/lang/Exception"), "");
            break;
        }
        default:{
			LOG("IN case default\n");
			//return throwOutOfMemoryError( env, "throwNoSuchMethodError: Default case" );
			return (*env)->ThrowNew(env, (*env)->FindClass(env, "java/lang/Exception"), "");
			printf("Msg = error\n");
		}
            
    }
		
	//mainloop();
	char str1[] = {0x1f,0x4,0x40,0x4,0x38, 0x4, 0x32, 0x4, 0x35, 0x4, 0x42, 0x4, 0x21, 0, 0, 0};
	jstring result = (*env)->NewString(env, (jchar*)str1,0);
	return result;

    //(*env)->NewStringUTF(env, "Hello From JNI");
	}
	
	JNIEXPORT jint JNICALL Java_com_example_lesah_1000_ndktest_Valera_DataAvailableJNI(JNIEnv * env, jobject obj){
		VbClipConnect();
		LOG("IN VbglR3ClipboardReportFormats\n");
		int rc = VbglR3ClipboardReportFormats(client, TEXT);
		jint ret = 0;
		return ret;
  }
  
  JNIEXPORT jint JNICALL Java_com_example_lesah_1000_ndktest_ClipListener_DataAvailableJNI(JNIEnv * env, jobject obj){
	  
		VbClipConnect();
		int rc = VbglR3ClipboardReportFormats(client, TEXT);
		jint ret = 0;
		return ret;
  }
  
  
JNIEXPORT jint JNICALL Java_com_example_lesah_1000_ndktest_MainActivity_DataAvailableJNI(JNIEnv * env, jobject obj){
	  	LOG("Main Activity Data Available connected = %d\n",connected );
		LOG("Main Activity Data Available client = %d\n",client );
		VbClipConnect();

		int rc = VbglR3ClipboardReportFormats(client, 1);
		jint ret = rc;
		return ret;
  }
  
  

  JNIEXPORT jint JNICALL Java_com_example_lesah_1000_ndktest_Valera_DataSendToHostJNI (JNIEnv * env, jobject obj, jstring str){
	VbClipConnect();
	jint ret = 0;
	//const jsize len      = (*env)->GetStringUTFLength(env, bitmappath);
	//printf("String valera sendig = %d\n", (int)len );
	LOG("Valera Data send to client\n");
	const char* chars = (*env)->GetStringChars(env, str, NULL);
	
	int i=0;
	if ( chars != 0){
		for(i = 0; i<=32000; i+=2){
			if (chars[i]==0 && chars[i+1] == 0 ){
				break;
			}
		}
		i+=2;
		int j;
		for( j = 0; j<i; j++){
			printf("%d = %d\n", j, chars[j]);
			
		}
		rc = VbglR3ClipboardWriteData( client, 1 , (void*) chars , i);
		ret= i;
	}else{
		ret = -1;
	}	

	//char str1[] = {0x1f,0x4,0x40,0x4,0x38, 0x4, 0x32, 0x4, 0x35, 0x4, 0x42, 0x4, 0x21, 0, 0, 0};
	//VbglR3ClipboardWriteData( client, 1 , str1 , 16);
	//printf("Valera Data send to client len = %d\n",i );
	//(*env)->ReleaseStringChars(str, chars);
	  
	  
	  return ret;
  }


