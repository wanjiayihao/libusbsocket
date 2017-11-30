package com.szty;

import android.net.LocalServerSocket;
import android.net.LocalSocket;
import android.os.Debug;
import android.provider.Settings;
import android.util.Log;

import java.net.Socket;

/**
 * Created by szty on 17-11-29.
 */

public class USBSocket {
    static {
        try {
            System.loadLibrary("usbsocket");
        }catch (Exception e){
            Log.d("device","load library error ",e);
        }
    }


    private  int    mSocket = 0;
    native private  boolean nativeIsDeviceConnect();
    private native  String nativeGetDeviceUDID();

    private native int  nativeConnectPort(int port);
    private native int nativeDisconnect(int handle);

    private native int nativeSend(int handle,byte buf[],int off,int len);
    private native int nativeReceive(int handle , byte buf[] , int off,int len);
    private native int nativeReceiveTimeout(int handle , byte buf[] , int off,int len,int  timeout);

    private native boolean nativeSubscribe();
    private native boolean nativeUnSubscribe();

    private static final boolean    DebugWifi= false;
    private static final String     ipaddress = "192.168.3.110";
    private Socket  mDebugSocket = null;





    public USBSocket(){
        Log.d("device"," iphone device:"+nativeGetDeviceUDID());
        //nativeSubscribe();
        startEventListener();
    }

    public String getDeviceUDID(){
        if(DebugWifi){
            return "debug wifi";
        }
        return nativeGetDeviceUDID();
    }
    public boolean isDeviceAvailable(){
        if(DebugWifi){
            return true;
        }
        return nativeIsDeviceConnect();
    }

    public boolean isDeviceConnected(){
        if(DebugWifi){
            if(mDebugSocket == null){
                return false;
            }else{
                return true;
            }
        }
        if(mSocket != 0){
            return true;
        }else{
            return false;
        }
    }

    public boolean connectPort(int port){
        if(DebugWifi){

            try {
                mDebugSocket = new Socket(ipaddress, port);


                return true;
            }catch (Exception e){
                Log.d("device","wifi socket error ",e);
            }
            return false;

        }
        mSocket = nativeConnectPort(port);
        if(mSocket >0){
            return true;
        }else{
            mSocket =0;
            return false;
        }
    }



    public int sendData(byte buf[],int off,int len){
        if(DebugWifi){
            try {
                mDebugSocket.getOutputStream().write(buf, off, len);
                return len;
            }catch (Exception e){
                Log.d("device","wifi socket error ",e);

            }
            return -1;
        }
        if(mSocket == 0){
            return -1;
        }
        return nativeSend(mSocket,buf,off,len);
    }

    public int receiveData(byte buf[], int off,int len){
        if(DebugWifi){
            try {
                return mDebugSocket.getInputStream().read(buf, off, len);
            }catch (Exception e){
                Log.d("device","wifi socket error ",e);

            }
            return -1;
        }
        if(mSocket == 0){
            return -1;
        }
        return nativeReceive(mSocket,buf,off,len);
    }
    public int receiveDataTimeout(byte buf[],int off,int len ,int timeout){
        if(DebugWifi){
            try {
                return mDebugSocket.getInputStream().read(buf, off, len);
            }catch (Exception e){
                Log.d("device","wifi socket error ",e);

            }
            return -1;
        }

        if(mSocket == 0){
            return -1;
        }
        return nativeReceiveTimeout(mSocket,buf,off,len,timeout);
    }

    public void disconnect(){
        if(DebugWifi){
            try {
                mDebugSocket.close();

            }catch (Exception e){
                Log.d("device","wifi socket error ",e);

            }
            mDebugSocket  = null;
       }
       if(mSocket == 0) {
           return;
       }

        nativeDisconnect(mSocket);
        mSocket= 0;

    }





    private static LocalServerSocket  mLocalServer = null;
    public void startEventListener(){
        if(mLocalServer != null){
            return ;
        }

        Thread thread = new Thread(){

            @Override
            public void run() {

                byte buf[] = new byte[128];
                try {
                    mLocalServer = new LocalServerSocket("com.szty.usbsocket.event");
                    nativeSubscribe();

                    while(true){
                        LocalSocket localSocket = mLocalServer.accept();
                        int len = localSocket.getInputStream().read(buf);
                        if(len > 0) {
                            String str = new String(buf, 0, len);
                            Log.d("device","java receive message :"+str);
                            String list[] =  str.split(";");
                            int event =  Integer.valueOf(list[0]);
                            int handle = Integer.valueOf(list[1]);
                            int product_id = Integer.valueOf(list[2]);
                            String udid = list[3];

                            if(mListener != null){
                                mListener.onEvent(event,handle,product_id,udid);
                            }
                        }

                        localSocket.close();
                    }

                }catch (Exception e){
                    Log.d("device","local server error ",e);
                }

            }
        };
        thread.start();
    }



    public interface DeviceEventListener{
        public void onEvent(int event,int handle , int product_id,String udid);
    }


    private DeviceEventListener  mListener = null;

    public void setDeviceEventListener(DeviceEventListener listener){
        mListener = null;
    }

    public static final int UE_DEVICE_ADD = 1;
    public static final int UE_DEVICE_REMOVE = 2;
    public static final int UE_DEVICE_PAIRED = 3;


}
