#include "com_example_lesah_000_ndktest_Valera.h"
#include "cli.h"

//#include "base/android/jni_string.h"
//#include "base/strings/utf_string_conversions.h"

/*
 * Class:     com_example_lesah_000_ndktest_MainActivity
 * Method:    HelloJNI
 * Signature: ()Ljava/lang/String;
 */


int des=0;
int connected = 0;
int rc = 0;
uint32_t Msg;
uint32_t u32Msg;
uint32_t fFormats;
uint32_t client;


JNIEXPORT jstring JNICALL Java_com_example_lesah_1000_ndktest_Valera_HelloJNI
  (JNIEnv *env, jobject obj){
	 
	if ( connected == 0 )
	{
		connected = 1;
		des = open("/dev/vboxguest", O_RDWR );
		printf("Descriptor = %d\n", des);
		VbglR3ClipboardConnect(&client);
	}
	rc = VbglR3ClipboardGetHostMsg(client, &Msg, &fFormats);
	switch( Msg)
        {
        case VBOX_SHARED_CLIPBOARD_HOST_MSG_FORMATS:
        {
            //printf("Msg = MSG_FORMATS\n");
            //printf("fFormats =  %u\n" , fFormats);
			char valera[10000];
            uint32_t count;
            rc = VbglR3ClipboardReadData(client, fFormats,  valera, 10000, &count);
            //printf("Count = %u\n" , count);
            //int i;
            //for ( i = 0; i < count; i++)
            //   { printf("%d %u ", i, (uint32_t) valera[i]);
            //    printf("%c\n", valera[i]);}
            //printf("\n");
			jstring result = (*env)->NewString(env, (jchar*)valera,count / 2 );
			return result;
            break;
        }
        case VBOX_SHARED_CLIPBOARD_HOST_MSG_READ_DATA:
        {
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
            printf("Msg = MSG_QUIT\n");
            break;
        }
        default:{
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
		
	  int rc = VbglR3ClipboardReportFormats(client, TEXT);
	  jint ret = 0;
	  return ret;
  }
  
  JNIEXPORT jint JNICALL Java_com_example_lesah_1000_ndktest_ClipListener_DataAvailableJNI(JNIEnv * env, jobject obj){
	  
	if ( connected == 0 )//&&&&&&&&&&&&&&&&&
	{
		connected = 1;
		des = open("/dev/vboxguest", O_RDWR );
		printf("Descriptor = %d\n", des);
		VbglR3ClipboardConnect(&client);
	}
	  
 	  int rc = VbglR3ClipboardReportFormats(client, TEXT);
	  jint ret = 0;
	  return ret;
  }
  
  
JNIEXPORT jint JNICALL Java_com_example_lesah_1000_ndktest_MainActivity_DataAvailableJNI(JNIEnv * env, jobject obj){
	  
	if ( connected == 0 )//&&&&&&&&&&&&&&&&&
	{
		connected = 1;
		des = open("/dev/vboxguest", O_RDWR );
		printf("Descriptor = %d\n", des);
		VbglR3ClipboardConnect(&client);
	}
	  printf("Main Activity Data Available connected = %d\n",connected );
	  printf("Main Activity Data Available client = %d\n",client );
 	  int rc = VbglR3ClipboardReportFormats(client, 1);
	  jint ret = rc;
	  return ret;
  }
  
  

  JNIEXPORT jint JNICALL Java_com_example_lesah_1000_ndktest_Valera_DataSendToHostJNI (JNIEnv * env, jobject obj, jstring str){
	
	jint ret = 0;
	//const jsize len      = (*env)->GetStringUTFLength(env, bitmappath);
	//printf("String valera sendig = %d\n", (int)len );
	printf("Valera Data send to client\n");
	const char* chars = (*env)->GetStringChars(env, str, NULL);
	
	int i=0;
	if ( chars != 0){
		for(i = 0; i<=32000; i+=2){
			if (chars[i]==0 && chars[i+1] == 0 ){
				break;
			}
		}
		int j;
		for( j = 0; j<i; j++){
			printf("%d = %d\n", j, chars[j]);
			
		}
		rc = VbglR3ClipboardWriteData( client, 1 , chars , i);
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


