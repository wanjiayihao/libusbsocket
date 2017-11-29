
#include <usbmuxd.h>

#include <pthread.h>


int mSocket = 0;
int mDeviceHandle = 0;
void  usbDeviceCallback (const usbmuxd_event_t *event, void *user_data){

	switch(event->event){
		case UE_DEVICE_ADD:
			printf("device add for %s   product_id %d \n",event->device.udid , event->device.product_id );
			mDeviceHandle = event->device.handle;
			startClient();
			break;
		case UE_DEVICE_REMOVE:

			printf("device removed for %s\n",event->device.udid );

			break;
		case UE_DEVICE_PAIRED:
			printf("device paired for %s\n",event->device.udid );

			break;
		default:
		break;
	}

}

pthread_t  stoc;

static void *run_stoc_loop(void *arg){

		mSocket =  usbmuxd_connect(mDeviceHandle, 2345);

		if(mSocket >0){
				const char * hello = "this is a hello test";
				uint32_t send_len;
				usbmuxd_send(mSocket, hello, strlen(hello)+1, &send_len);

				printf("send str len %d \n",send_len);


				char  buffer[64] = {0} ;
				int i;
				while(1){
					int rt = usbmuxd_recv_timeout(mSocket,buffer,20,&send_len,1000);
					if(rt ==0){
						printf("receive data ok len %d :%s \n",send_len,buffer);
					}else{
						printf("receive data error \n");
						break;
					}
				}

				usbmuxd_disconnect(mSocket);

		}

		exit(0);
		return NULL;
}
void startClient(){


	usbmuxd_unsubscribe();
	stoc = pthread_create(&stoc, NULL, run_stoc_loop, NULL);
	pthread_join(stoc, NULL);




}



int main(int argc , const char * argv){

	printf("test usb callback \n");
	//usbmuxd_subscribe(usbDeviceCallback,NULL);
	

	usbmuxd_device_info_t *dev_list = NULL;

	int count = usbmuxd_get_device_list(&dev_list);
	if (count  < 0) {
		printf("Connecting to usbmuxd failed, terminating.\n");
		free(dev_list);
		
		return 0;
	}

	printf( "Number of available devices == %d\n", count);

	mDeviceHandle = dev_list->handle;

	startClient();


	while(1){
		sleep(1);
	}
	return 0;
}