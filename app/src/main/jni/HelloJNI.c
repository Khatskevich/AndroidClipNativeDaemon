#include "com_example_lesah_000_ndktest_MainActivity.h"
#include "cli.h"
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


JNIEXPORT jstring JNICALL Java_com_example_lesah_1000_ndktest_MainActivity_HelloJNI
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
            printf("Msg = MSG_FORMATS\n");

            char valera[10000];
            uint32_t count;
            printf("fFormats =  %u\n" , fFormats);
            rc = VbglR3ClipboardReadData(client, fFormats,  valera, 10000, &count);
            printf("Count = %u\n" , count);
            int i;
            /*for ( i = 0; i < count; i++)
               { printf("%d %u ", i, (uint32_t) valera[i]);
                printf("%c\n", valera[i]);}
            printf("\n");*/
			(*env)->NewStringUTF(env, valera);
			return;
            break;
        }
        case VBOX_SHARED_CLIPBOARD_HOST_MSG_READ_DATA:
        {
            printf("Msg = MSG_READ_DATA\n");
			//int VbglR3ClipboardWriteData(uint32_t u32ClientId, uint32_t fFormat, void *pv, uint32_t cb)
			char valera[] = {'B', '\0',' ', '\0','s', '\0','o', '\0','s', '\0','e', '\0','t', '\0','0', '\0', };
			rc = VbglR3ClipboardWriteData( client, 1 , valera , 16);
            break;
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
    (*env)->NewStringUTF(env, "Hello From JNI");
	
  }



