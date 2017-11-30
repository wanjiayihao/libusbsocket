package com.szty;

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


    private static final boolean    DebugWifi= true;
    private static final String     ipaddress = "192.168.3.110";
    private Socket  mDebugSocket = null;

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

}
