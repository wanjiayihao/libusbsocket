
#include <usbmuxd.h>



void  usbDeviceCallback (const usbmuxd_event_t *event, void *user_data){

	switch(event->event){
		case UE_DEVICE_ADD:
			printf("device add for %s\n",event->device.udid );
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

int main(int argc , const char * argv){

	printf("test usb callback \n");
	usbmuxd_subscribe(usbDeviceCallback,NULL);
	while(1){
		sleep(1);
	}
	usbmuxd_unsubscribe();
	return 0;
}