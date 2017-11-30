

#include <android_runtime/AndroidRuntime.h>
#include <binder/IBinder.h>
#include <binder/IPCThreadState.h>
#include <binder/IServiceManager.h>
#include <utils/Log.h>
#include <utils/misc.h>
#include <binder/Parcel.h>
#include <utils/threads.h>
#include <cutils/properties.h>

#ifdef __cplusplus
extern "C" {
#endif


static int javaDetachThread(void)
{
    JavaVM* vm;
    jint result;

    vm = android::AndroidRuntime::getJavaVM();
    assert(vm != NULL);

    result = vm->DetachCurrentThread();
    if (result != JNI_OK)
        ALOGE("ERROR: thread detach failed\n");
    return result;
}


#ifdef __cplusplus
}
#endif
