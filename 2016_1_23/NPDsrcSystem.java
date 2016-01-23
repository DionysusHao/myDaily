/*

* (c) PIONEER CORPORATION 2014

* 25-1 Nishi-machi Yamada Kawagoe-shi Saitama-ken 350-8555 Japan

* All Rights Reserved.

*/
package jp.pioneer.ceam.handler;

import java.lang.ref.WeakReference;
import android.util.Log;
import android.os.Looper;
import android.os.Message;
import android.os.Handler;
import android.os.Parcel;
import java.lang.Byte;
import java.lang.Long;
public class NPDsrcSystem {
    private static final String TAG = "NPDsrcSystem_java";

    static {
        Log.d ( TAG, "load the lib" );
        System.loadLibrary ( "NPDsrcSystem_JNI" );
        Log.d ( TAG, "load success" );
        native_init();
    }

    private Handler mHandler = null;

    private class EventHandler extends Handler {
        private NPDsrcSystem mSystem;
        public EventHandler ( NPDsrcSystem ms, Looper looper ) {
            super ( looper );
            mSystem = ms;
        }

        @Override
        public void handleMessage ( Message msg ) {
            if ( mEventListener != null ) {
                //get data
                Parcel obj = ( Parcel ) msg.obj;

                //msg.what--> function id.
                if ( obj != null && msg.obj instanceof Parcel ) {
                    Log.d ( TAG, "handleMessage::function id=" + msg.what );
                    Log.d ( TAG, "obj.dataSize() =" + obj.dataSize() );

                    //set the position to 0 just for unexpected changing
                    obj.setDataPosition ( 0 );
                    mEventListener.onReceiveNPDsrcEvent ( msg.what, obj.createByteArray() );
                    obj.recycle();
                }
            } else {
                Log.d ( TAG, "Have not set listener yet" );
            }
        }
    }

    public interface NPDsrcEventListener {
        public void onReceiveNPDsrcEvent ( int eventID, byte[] eventData );
    }

    public void setOnNPDsrcEventListener ( NPDsrcEventListener listener ) {
        mEventListener = listener;
    }

    private NPDsrcEventListener mEventListener = null;


    /*
     *Define all the data type used by the API functions
     */
    public static class NDsrcEvent {
        long    m_when;
        long    m_ev_id;
    };

    public static class NDsrcPushResponse {
        byte bPushId;
        NDsrcPushAcknowledgement pnAcknowledge;

        public NDsrcPushResponse ( byte[] dataBody ) {
            Log.d ( TAG, "java::NDsrcPushResponse constructor" );
            bPushId = 0;

            if ( null == dataBody ) {
                pnAcknowledge.dwDataSize = 0;
                pnAcknowledge.pbDataBody = null;
            } else {
                pnAcknowledge = new NDsrcPushAcknowledgement ( dataBody );
            }
        }
    };

    private static class NDsrcPushAcknowledgement {
        long dwDataSize;
        byte[] pbDataBody ;

        private NDsrcPushAcknowledgement ( byte[] dataBody ) {
            Log.d ( TAG, "java::NDsrcPushAcknowledgement constructor" );

            dwDataSize = dataBody.length;

            if ( dwDataSize <= 0 || dwDataSize > Long.MAX_VALUE ) {
                Log.e ( TAG, "java::NDsrcPushResponse constructor: he length of the byte array can not be 0 ,negative or larger than the max value of an Long!" );
                pbDataBody = null;
                dwDataSize = 0;
                return ;
            }

            pbDataBody = dataBody;
        }
    };

    //counterpart of the length of abDataBody in native level
    public static final int NDSRC_SUPPLEMENT_INFO_SIZE = 128;
    public static final int NDSRC_PUSH_ABORT_ERR_OTHER = 0xFF;
    public static class NDsrcPushAbort {
        byte bPushId;
        int eStatus;//an enum
        NDsrcSupplementInfo pnSuppInfo;

        public NDsrcPushAbort ( byte[] dataBody ) {
            Log.d ( TAG, "java::NDsrcPushResponse constructor" );
            bPushId = 0;
            eStatus = 0;

            if ( null == dataBody ) {
                pnSuppInfo.bDataSize = 0;
                pnSuppInfo.abDataBody = null;
                eStatus = NDSRC_PUSH_ABORT_ERR_OTHER;
            } else {
                pnSuppInfo = new NDsrcSupplementInfo ( dataBody );
            }

            pnSuppInfo = new NDsrcSupplementInfo ( dataBody );
        }
    };

    private static class NDsrcSupplementInfo {
        byte bDataSize;
        byte [] abDataBody = new byte[NDSRC_SUPPLEMENT_INFO_SIZE];

        public NDsrcSupplementInfo ( byte[] dataBody ) {
            Log.d ( TAG, "java::NDsrcPushAcknowledgement constructor" );

            if ( dataBody.length <= 0 || dataBody.length > Byte.MAX_VALUE ) {
                Log.e ( TAG, "java::NDsrcPushAcknowledgement constructor: he length of the byte array can not be 0 ,negative or larger than the max value of an byte!" );
                abDataBody = null;
                bDataSize = 0;
                return ;
            }

            bDataSize = ( byte ) dataBody.length;
            abDataBody = dataBody;
        }
    }

    //const for the next two classes
    static final int NDSRC_MEM_TAG_SIZE = 2;
    public static class NDsrcMemAllocInfo {
        long[] adwMemTag;
        boolean blSpf;
        boolean blWriteProtect;
        boolean blReadProtect;
        long dwMaxMemorySize;
        long dwDataSize;
        byte [] pbDataBody;

        public NDsrcMemAllocInfo ( byte[] dataBody ) {
            Log.d ( TAG, "java::NDsrcMemAllocInfo constructor" );

            adwMemTag = new long [NDSRC_MEM_TAG_SIZE];
            blSpf = false;
            blWriteProtect = false;
            blReadProtect = false;
            dwMaxMemorySize = 0;

            if ( null == dataBody ) {
                Log.e ( TAG, "java::NDsrcMemAllocInfo constructor: the byte array cannot be null" );
                return ;
            }

            if ( dataBody.length <= 0 || dataBody.length > Long.MAX_VALUE ) {
                Log.e ( TAG, "java::NDsrcMemAllocInfo constructor: he length of the byte array can not be 0 ,negative or larger than the max value of an Long!" );
                pbDataBody = null;
                dwDataSize = 0;
                return ;
            }

            dwDataSize = dataBody.length;
            pbDataBody = dataBody;
        }
    };

    public static class NDsrcMemData {
        long[] adwMemTag;
        long dwDataSize;
        byte[]  pbDataBody;

        public NDsrcMemData ( byte[] dataBody ) {
            Log.d ( TAG, "java::NDsrcMemData constructor" );
            adwMemTag = new long [NDSRC_MEM_TAG_SIZE];

            if ( null == dataBody ) {
                Log.e ( TAG, "java::NDsrcMemData constructor: the byte array cannot be null" );
                return ;
            }

            if ( dataBody.length <= 0 || dataBody.length > Long.MAX_VALUE ) {
                Log.e ( TAG, "java::NDsrcMemData constructor: he length of the byte array can not be 0 ,negative or larger than the max value of an Long!" );
                pbDataBody = null;
                dwDataSize = 0;
                return ;
            }

            dwDataSize = dataBody.length;
            pbDataBody = dataBody;
        }
    };
    public static class NDsrcMemMibData {
        int eMibId;// an enum
        long dwMibParam;
    };

    //singleton
    private NPDsrcSystem() {
        Log.d ( TAG, "java::NPDsrcSystem constructor" );

        Looper looper;

        if ( ( looper = Looper.myLooper() ) != null ) {
            mHandler = new EventHandler ( this, looper );
        } else if ( ( looper = Looper.getMainLooper() ) != null ) {
            mHandler = new EventHandler ( this, looper );
        } else {
            mHandler = null;
            Log.e ( TAG, "get handler failed" );
        }

        native_setup ( new WeakReference<NPDsrcSystem> ( this ) );
    }

    private static NPDsrcSystem mSystem = null;

    public static NPDsrcSystem getInstance() {
        Log.d ( TAG, "java::NPDsrcSystem.getInstance()" );

        if ( mSystem == null ) {
            mSystem = new NPDsrcSystem();
        }

        return mSystem;
    }


    private native static  final void native_init();
    private native final void native_setup ( Object weakRef );
    //private native final void native_finalize();

    /*
     * API fucntions
     */
    public native boolean GetCurrentEvent ( NDsrcEvent pnEvent );

    public boolean SendPushResponse ( NDsrcPushResponse pnPushRes, long dwSeqNo, int wPortNo, int
                                      wHandleNo ) {
        Log.d ( TAG, "Java:: SendPushResponse" );

        if ( null != pnPushRes.pnAcknowledge && null != pnPushRes.pnAcknowledge.pbDataBody ) {
            return SendPushResponse ( pnPushRes.bPushId, pnPushRes.pnAcknowledge.pbDataBody,
                                      dwSeqNo, wPortNo, wHandleNo );
        } else {
            Log.d ( TAG, "pnPushRes.pnAcknowledge orpnPushRes.pnAcknowledge.pbDataBody is null!" );
            return SendPushResponse ( pnPushRes.bPushId, null, dwSeqNo, wPortNo, wHandleNo );
        }
    }

    private native boolean SendPushResponse ( byte bPushId, byte[] pbDataBody, long dwSeqNo, int wPortNo, int
            wHandleNo );

    public boolean SendPushRerunResponse ( NDsrcPushResponse pnPushRes, long dwSeqNo, int wPortNo, int
                                           wHandleNo ) {
        Log.d ( TAG, "Java:: SendPushRerunResponse" );

        if ( null != pnPushRes.pnAcknowledge && null != pnPushRes.pnAcknowledge.pbDataBody ) {
            return SendPushResponse ( pnPushRes.bPushId, pnPushRes.pnAcknowledge.pbDataBody,
                                      dwSeqNo, wPortNo, wHandleNo );
        } else {
            Log.d ( TAG, "pnPushRes.pnAcknowledge or pnPushRes.pnAcknowledge.pbDataBody is null!" );
            return SendPushResponse ( pnPushRes.bPushId, null, dwSeqNo, wPortNo, wHandleNo );
        }
    }

    private native boolean SendPushRerunResponse ( byte bPushId, byte[] pbDataBody, long dwSeqNo, int wPortNo, int
            wHandleNo );

    public boolean AbortPush ( NDsrcPushAbort pnPushAbort, long dwSeqNo, int wPortNo , int
                               wHandleNo ) {
        Log.d ( TAG, "Java:: AbortPush" );

        if ( null != pnPushAbort.pnSuppInfo && null != pnPushAbort.pnSuppInfo.abDataBody ) {

            return AbortPush ( pnPushAbort.bPushId, pnPushAbort.eStatus, pnPushAbort.pnSuppInfo.abDataBody,
                               dwSeqNo, wPortNo, wHandleNo );
        } else {
            Log.d ( TAG, "pnPushAbort.pnSuppInfo or pnPushAbort.pnSuppInfo.abDataBody is null!" );
            return AbortPush ( pnPushAbort.bPushId, pnPushAbort.eStatus, null,
                               dwSeqNo, wPortNo, wHandleNo );
        }
    }

    private native boolean AbortPush ( byte bPushId, int eStatus, byte[] abDataBody, long dwSeqNo, int wPortNo , int wHandleNo );

    public boolean AllocateMemory ( NDsrcMemAllocInfo pnDsrcMemAllocInfo, long dwSeqNo ) {
        Log.d ( TAG, "Java:: AllocateMemory" );

        if ( null == pnDsrcMemAllocInfo.pbDataBody ) {
            Log.d ( TAG, "pnDsrcMemAllocInfo.pbDataBody is null!" );
        }

        return AllocateMemory ( pnDsrcMemAllocInfo.adwMemTag, pnDsrcMemAllocInfo, pnDsrcMemAllocInfo.pbDataBody, dwSeqNo );
    }
    private native boolean AllocateMemory ( long[] adwMemTag, NDsrcMemAllocInfo pnDsrcMemAllocInfo, byte []pbDataBody, long dwSeqNo );


    public boolean WriteMemory ( NDsrcMemData pnDsrcMemData, long
                                 dwSeqNo ) {
        Log.d ( TAG, "Java:: WriteMemory" );

        if (  null != pnDsrcMemData.pbDataBody ) {
            Log.d ( TAG, "pnDsrcMemData.pbDataBody is null!" );
        }

        return WriteMemory ( pnDsrcMemData.adwMemTag, pnDsrcMemData.pbDataBody, dwSeqNo );
    }
    private native boolean WriteMemory ( long[]adwMemTag, byte[]pbDataBody, long dwSeqNo );

    public native boolean SetMemoryMibData ( NDsrcMemMibData pnDsrcMemMibData, long dwSeqNo );



    public native boolean StartLogging ( String pszDirName );
    public native boolean StopLogging();

    private static void postEventFromNative ( Object ref,
            int what, int arg1, int arg2, Object obj ) {
        Log.d ( TAG, "postEventFromNative what = " + what );

        NPDsrcSystem mSystem = ( NPDsrcSystem ) ( ( WeakReference ) ref ).get();

        if ( mSystem == null ) {
            Log.e ( TAG, "NPVicsFmSystem died" );
            return;
        }

        if ( mSystem.mHandler != null ) {
            Message msg = mSystem.mHandler.obtainMessage ( what, arg1,
                          arg2, obj );
            mSystem.mHandler.sendMessage ( msg );
        } else {
            Log.e ( TAG, "handler is null" );
        }
    }
}
