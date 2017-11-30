/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <string.h>


#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <netinet/in.h>
#include <time.h>

#include <usbmuxd.h>



#include <jni.h>


/* This is a trivial JNI example where we use a native method
 * to return a new VM String. See the corresponding Java source
 * file located at:
 *
 *   apps/samples/hello-jni/project/src/com/example/hellojni/HelloJni.java
 */

jboolean  Java_com_szty_USBSocket_nativeIsDeviceConnect(JNIEnv* env , jobject thiz )
{

	usbmuxd_device_info_t *dev_list = NULL;

	int count = usbmuxd_get_device_list(&dev_list);

	if(dev_list){
		free(dev_list);
	}
	if ( count > 0) {
		return JNI_TRUE;
	}else{
		return JNI_FALSE;
	}
}


jstring  Java_com_szty_USBSocket_nativeGetDeviceUDID(JNIEnv* env , jobject thiz )
{

	usbmuxd_device_info_t *dev_list = NULL;

	int count = usbmuxd_get_device_list(&dev_list);
	jstring str =  NULL;

	if(dev_list&&count>0){
		str = (*env)->NewStringUTF(env,(const jchar*)dev_list->udid);
		free(dev_list);
	}else{
		if(dev_list){
			free(dev_list);
		}
		str = (*env)->NewStringUTF(env,(const jchar*)"no device");
	}
	return str;
}


jint  Java_com_szty_USBSocket_nativeConnectPort(JNIEnv* env , jobject thiz ,jint port )
{

	usbmuxd_device_info_t *dev_list = NULL;

	int count = usbmuxd_get_device_list(&dev_list);
	int handler = -1;
	if(count <1 || dev_list == NULL){
		if(dev_list){
			free(dev_list);
		}
		return handler;
	}
	
	handler = usbmuxd_connect(dev_list->handle, port);
	free(dev_list);

	return handler;


}



jint  Java_com_szty_USBSocket_nativeDisconnect(JNIEnv* env , jobject thiz ,jint handler )
{
	return usbmuxd_disconnect(handler);
}

jint  Java_com_szty_USBSocket_nativeSend(JNIEnv* env , jobject thiz ,jint handler , jbyteArray bArray,
                                                jint off, jint len)
{
	jbyte* b = (*env)->GetByteArrayElements(env,bArray, NULL);
	const char * data  = (const char *)b;
	uint32_t sent_bytes = 0 ;
	int ret =  usbmuxd_send(handler, data+off, len , &sent_bytes);

	(*env)->ReleaseByteArrayElements(env,bArray, b, 0);

	if(ret == 0){
		return sent_bytes;
	}else{
		return ret;
	}
}

jint  Java_com_szty_USBSocket_nativeReceive(JNIEnv* env , jobject thiz ,jint handler , jbyteArray bArray,jint off , jint len)
{
	jbyte* b = (*env)->GetByteArrayElements(env,bArray, NULL);
	uint32_t recv_bytes=0;
	char * data = (char *) b;
	int ret = usbmuxd_recv(handler, data + off,  len, &recv_bytes);
	(*env)->ReleaseByteArrayElements(env,bArray, b, 0);

	if(ret ==0){
		return recv_bytes;
	}else{
		return ret;
	}
}

jint  Java_com_szty_USBSocket_nativeReceiveTimeout(JNIEnv* env , jobject thiz ,jint handler , jbyteArray bArray ,jint off , jint len,jint timeout)
{
	jbyte* b = (*env)->GetByteArrayElements(env,bArray, NULL);
	uint32_t recv_bytes=0;
	char * data = (char *) b;

	int ret = usbmuxd_recv_timeout(handler, data+off,  len, &recv_bytes,timeout);

	(*env)->ReleaseByteArrayElements(env,bArray, b, 0);

	if(ret ==0){
		return recv_bytes;
	}else{
		return ret;
	}

}

 static JavaVM*  jvm = NULL;
 static jobject ObjExtern = NULL;
 static jmethodID method_onEvent = NULL;

static void  usbDeviceCallback (const usbmuxd_event_t *event, void *user_data){

	JNIEnv* env = NULL ;

	(*jvm)->AttachCurrentThread(jvm,&env,NULL);
	if(env == NULL){
		return ;
	}

 	jclass clazz;
    clazz = (*env)->FindClass(env,"com/szty/USBSocket");
    if(clazz == NULL){
    	return ;
    }

	method_onEvent =  (*env)->GetStaticMethodID(env,clazz, "eventCallback", "(IIILjava/lang/String;)V");
	if(method_onEvent == NULL){
		return ;
	}

	jstring udid = (*env)->NewStringUTF(env,(const jchar*)event->device.udid);
	(*env)->CallStaticVoidMethod(env, clazz ,method_onEvent, event->event, event->device.handle,event->device.product_id,udid );

	if(udid != NULL){
		(*env)->DeleteLocalRef(env,udid);
	}

	(*jvm)->DetachCurrentThread(jvm);
}


jboolean  Java_com_szty_USBSocket_nativeSubscribe(JNIEnv* env , jobject thiz)
{
	    jclass clazz;
    	clazz = (*env)->FindClass(env,"com/szty/USBSocket");
    	if(clazz == NULL){
    		return JNI_FALSE;
    	}


		(*env)->GetJavaVM(env,&jvm);

		if(jvm == NULL){
			return JNI_FALSE;
		}


		// usbmuxd_event_t event;
		// event.event = 1;
		// event.device.handle = 2;
		// event.device.product_id = 3;
		// strcpy(event.device.udid,"hello test");

		//usbDeviceCallback(&event,NULL);
		//(*env)->CallVoidMethod(env,thiz, method_onEvent, event.event, event.device.handle,event.device.product_id,NULL );


		//return JNI_TRUE;
		int ret  = usbmuxd_subscribe(usbDeviceCallback,NULL);
		if(ret ==0){
			return JNI_TRUE;
		}else{
			return JNI_FALSE;
		}
}

void  Java_com_szty_USBSocket_nativeUnSubscribe(JNIEnv* env , jobject thiz){
	usbmuxd_unsubscribe();
}




