/*
* (c) PIONEER CORPORATION 2014
* 25-1 Nishi-machi Yamada Kawagoe-shi Saitama-ken 350-8555 Japan
* All Rights Reserved.
*/

#define LOG_TAG "DSRCSystem-JNI"

#include <jni.h>
#include <JNIHelp.h>
#include <android_runtime/AndroidRuntime.h>
#include <binder/Parcel.h>

#include "android_os_Parcel.h"
#include "NPDsrcSystem.h"
#include "SysdclientStub.h"
#include "NEventID.h"

using namespace MediaLibrary;
using namespace android;

static const char * const kClassNPDsrcSystem = "jp/pioneer/ceam/handler/NPDsrcSystem";
static const char * const kClassNDsrcClient = "jp/pioneer/ceam/handler/NPDsrcSystem$NDsrcClient";


struct fields_t {
    jclass      mClass;
    jfieldID    dwSeq;
    jmethodID   mPost_event;
};
static struct fields_t fields = {NULL, NULL , NULL};
static Mutex gLock;



class DsrcJniListener : public SysdclientStub {
    //onreveive functions which get event notified from native functions
    void OnDsrcPushSendResultEvent ( const NDsrcPushSendResultEvent& ev );
    void OnDsrcMemAllocEvent ( const NDsrcMemAllocEvent& ev );
    void OnDsrcMemWriteEvent ( const NDsrcMemWriteEvent& ev );
    void OnDsrcMemMibEvent ( const NDsrcMemMibEvent& ev );

    //other onreceive functions reletive to Dsrc,catching the logs below means we get event notifies that have not been handled yet
    void OnDsrcStatusEvent ( const NDsrcStatusEvent& ev ) {
        LOGW ( "OnDsrcStatusEvent,not used yet!" );
    }
    void OnDsrcPushEvent ( const NDsrcPushEvent& ev ) {
        LOGW ( "OnDsrcPushEvent,not used yet!" );
    }
    void OnDsrcPushRerunEvent ( const NDsrcPushRerunEvent& ev ) {
        LOGW ( "OnDsrcPushRerunEvent,not used yet!" );
    }
    void OnDsrcPushAbortEvent ( const NDsrcPushAbortEvent& ev ) {
        LOGW ( "OnDsrcPushAbortEvent,not used yet!" );
    }
    void OnDsrcPushInitEvent ( const NDsrcPushInitEvent& ev ) {
        LOGW ( "OnDsrcPushInitEvent,not used yet!" );
    }
    void OnDsrcMemAccessInfoEvent ( const NDsrcMemAccessInfoEvent& ev ) {
        LOGW ( "OnDsrcMemAccessInfoEvent,not used yet!" );
    }
    void OnDsrcLpcpConnectEvent ( const NDsrcLpcpConnectEvent& ev ) {
        LOGW ( "OnDsrcLpcpConnectEvent,not used yet!" );
    }
    void OnDsrcSpfCertificateEvent ( const NDsrcSpfCertificateEvent& ev ) {
        LOGW ( "OnDsrcSpfCertificateEvent,not used yet!" );
    }
    void OnDsrcGetUnitRegistrationInfoEvent ( const NDsrcGetUnitRegistrationInfoEvent& ev ) {
        LOGW ( "OnDsrcGetUnitRegistrationInfoEvent,not used yet!" );
    }
    void OnDsrcPppcpConnectEvent ( const NDsrcPppcpConnectEvent& ev ) {
        LOGW ( "OnDsrcPppcpConnectEvent,not used yet!" );
    }
    void OnDsrcSmartPullEvent ( const NDsrcSmartPullEvent& ev ) {
        LOGW ( "OnDsrcSmartPullEvent,not used yet!" );
    }
    void OnDsrcIndicationEvent ( const NDsrcIndicationEvent& ev ) {
        LOGW ( "OnDsrcIndicationEvent,not used yet!" );
    }
    void OnDsrcConfirmEvent ( const NDsrcConfirmEvent& ev ) {
        LOGW ( "OnDsrcConfirmEvent,not used yet!" );
    }
    void OnDsrcIndicationResultEvent ( const NDsrcIndicationResultEvent& ev ) {
        LOGW ( "OnDsrcIndicationResultEvent,not used yet!" );
    }

public:
    DsrcJniListener ( jobject obj, jobject weak_this );
    ~DsrcJniListener();
private:
    jclass mClass;
    jobject mObjectWeakRef;
};

DsrcJniListener::DsrcJniListener ( jobject thiz, jobject weak_this ) {
    LOGD ( "DsrcJniListener:: constructor!" );
    JNIEnv *env = AndroidRuntime::getJNIEnv();

    if ( env != NULL ) {

        jclass clazz = env->GetObjectClass ( thiz );

        if ( clazz == NULL ) {
            LOGE ( "JNI:: can not find class from object" );
            return;
        } else {
            mClass = ( jclass ) env->NewGlobalRef ( clazz );

            if ( mClass == NULL ) {
                LOGE ( "null point" );
                return;
            }
        }

        mObjectWeakRef  = env->NewGlobalRef ( weak_this );

        if ( mObjectWeakRef == NULL ) {
            LOGE ( "null point" );
            return;
        }

        env->DeleteLocalRef ( clazz );

    } else {
        LOGE ( "can not get env" );
    }
}

DsrcJniListener::~DsrcJniListener() {
    LOGD ( " DsrcJniListener construct\n" );
    JNIEnv *env = AndroidRuntime::getJNIEnv();

    if ( env != NULL ) {
        if ( mClass != NULL ) {
            LOGD ( "\n delete global refrence of mclass\n" );
            env->DeleteGlobalRef ( mClass );
            mClass = NULL;
        }

        if ( mObjectWeakRef != NULL ) {
            LOGD ( "\n delete global refrence of mObjectWeakRef\n" );
            env->DeleteGlobalRef ( mObjectWeakRef );
            mObjectWeakRef = NULL;
        }
    } else {
        LOGE ( "can not get env" );
    }
}

void DsrcJniListener::OnDsrcPushSendResultEvent ( const NDsrcPushSendResultEvent& ev ) {
    int eventId = ev.GetEventID();
    LOGD ( "OnDsrcPushSendResultEvent, eventID = %d", eventId );

    Mutex::Autolock l ( gLock );
    JNIEnv *env = AndroidRuntime::getJNIEnv();

    if ( NULL == env ) {
        LOGE ( "OnDsrcPushSendResultEvent:: can not find env!" );
    }
	
    // creat a new buff, the length of the buff must at least equal to the value buff_sz below
    // the member function Pack() cannot be used here because it miss a "const" in the end of its prototype
    unsigned int buff_sz  = sizeof ( ev.m_when ) + sizeof ( ev.eReqType ) + sizeof ( ev.eStatus )  + sizeof ( ev.dwSeqNo );
    BYTE * buff = new BYTE[buff_sz];
    INT nOffset = 0;
    memcpy ( &buff[ nOffset], &ev.m_when, sizeof ( ev.m_when ) );
    nOffset += sizeof ( ev.m_when );
    memcpy ( &buff[ nOffset], &ev.eReqType, sizeof ( ev.eReqType ) );
    nOffset += sizeof ( ev.eReqType );
    memcpy ( &buff[ nOffset], &ev.eStatus, sizeof ( ev.eStatus ) );
    nOffset += sizeof ( ev.eStatus );
    memcpy ( &buff[ nOffset], &ev.dwSeqNo, sizeof ( ev.dwSeqNo ) );
    nOffset += sizeof ( ev.dwSeqNo );

    // use Parcel to send the data to java level
    jobject jParcel = createJavaParcelObject ( env );

    if ( jParcel != NULL ) {
        Parcel* nativeParcel = parcelForJavaObject ( env, jParcel );

        if ( buff != NULL  ) {
            Parcel * tempParcel = new Parcel;
            tempParcel->setData ( buff, buff_sz );
            delete buff;
            LOGD ( "OnDsrcPushSendResultEvent::tempParcel.->dataSize() =%d", tempParcel->dataSize() );
            nativeParcel->writeInt32 ( tempParcel->dataSize() );
            nativeParcel->write ( tempParcel->data(), tempParcel->dataSize() );
            delete tempParcel;
            nativeParcel->setDataPosition ( 0 );
        } else {
            LOGE ( "OnDsrcPushSendResultEvent::NO DATA!" );
            return;
        }

        LOGD ( "OnDsrcPushSendResultEvent::nativeParcel->dataSize() =%d", nativeParcel->dataSize() );
        env->CallStaticVoidMethod ( fields.mClass, fields.mPost_event, mObjectWeakRef,
                                    eventId, 0, 0, jParcel );
        env->DeleteLocalRef ( jParcel );
    } else {
        LOGE ( "OnDsrcPushSendResultEvent::create JavaParcel failed" );
        return;
    }
}

void DsrcJniListener::OnDsrcMemAllocEvent ( const NDsrcMemAllocEvent& ev ) {
    int eventId = ev.GetEventID();
    LOGD ( "OnDsrcMemAllocEvent, eventID = %d", eventId );

    Mutex::Autolock l ( gLock );
    JNIEnv *env = AndroidRuntime::getJNIEnv();

    if ( NULL == env ) {
        LOGE ( "OnDsrcMemAllocEvent:: can not find env!" );
    }

    //Do want the member fucntion Pack do cause it miss "const" in the end of  its prototype
    unsigned int buff_sz;

    if ( NDSRC_MEM_SUCCESS != ev.eStatus ) {
        buff_sz = sizeof ( ev.m_when ) + sizeof ( ev.eStatus ) + sizeof ( ev.adwMemTag ) + sizeof ( *ev.pnSuppInfo )+ sizeof ( ev.dwSeqNo );
    } else {
        buff_sz = sizeof ( ev.m_when ) + sizeof ( ev.eStatus ) + sizeof ( ev.adwMemTag ) + sizeof ( ev.dwSeqNo );
    }

    LOGD ( "the size of the buff of OnDsrcMemAllocEvent = %d", buff_sz );
    BYTE * buff = new BYTE [buff_sz];

    INT nOffset = 0;
    memcpy ( &buff[ nOffset], &ev.m_when, sizeof ( ev.m_when ) );
    nOffset += sizeof ( ev.m_when );
    memcpy ( &buff[ nOffset], &ev.eStatus, sizeof ( ev.eStatus ) );
    nOffset += sizeof ( ev.eStatus );
    memcpy ( &buff[ nOffset], &ev.adwMemTag, sizeof ( ev.adwMemTag ) );
    nOffset += sizeof ( ev.adwMemTag );
    memcpy ( &buff[ nOffset], &ev.dwSeqNo, sizeof ( ev.dwSeqNo ) );
    nOffset += sizeof ( ev.dwSeqNo );

    if ( NDSRC_MEM_SUCCESS != ev.eStatus ) {
        memcpy ( &buff[ nOffset], &ev.pnSuppInfo->bDataSize, sizeof ( ev.pnSuppInfo->bDataSize ) );
        nOffset += sizeof ( ev.pnSuppInfo->bDataSize );
        memcpy ( &buff[ nOffset], ev.pnSuppInfo->abDataBody, ev.pnSuppInfo->bDataSize );
        nOffset += ev.pnSuppInfo->bDataSize;
    }

    jobject jParcel = createJavaParcelObject ( env );

    if ( jParcel != NULL ) {
        Parcel* nativeParcel = parcelForJavaObject ( env, jParcel );

        if ( buff != NULL  ) {
            Parcel * tempParcel = new Parcel;
            tempParcel->setData ( buff, buff_sz );
            delete buff;
            LOGD ( "OnDsrcMemAllocEvent::tempParcel.->dataSize() =%d", tempParcel->dataSize() );
            nativeParcel->writeInt32 ( tempParcel->dataSize() );
            nativeParcel->write ( tempParcel->data(), tempParcel->dataSize() );
            delete tempParcel;
            nativeParcel->setDataPosition ( 0 );
        } else {
            LOGE ( "OnDsrcMemAllocEvent::NO DATA!" );
            return;
        }

        LOGD ( "OnDsrcMemWriteEvent::nativeParcel->dataSize() =%d", nativeParcel->dataSize() );
        env->CallStaticVoidMethod ( fields.mClass, fields.mPost_event, mObjectWeakRef,
                                    eventId, 0, 0, jParcel );
        env->DeleteLocalRef ( jParcel );
    } else {
        LOGE ( "OnDsrcMemWriteEvent::create JavaParcel failed" );
        return;
    }
}

void DsrcJniListener::OnDsrcMemWriteEvent ( const NDsrcMemWriteEvent& ev ) {
    int eventId = ev.GetEventID();
    LOGD ( "OnDsrcMemWriteEvent, eventID = %d", eventId );

    Mutex::Autolock l ( gLock );
    JNIEnv *env = AndroidRuntime::getJNIEnv();

    if ( NULL == env ) {
        LOGE ( "OnDsrcMemWriteEvent:: can not find env!" );
    }

    // creat a new buff, the length of the buff must at least equal to the value buff_sz below
    unsigned int buff_sz ;

    if ( NDSRC_MEM_SUCCESS != ev.eStatus ) {
        buff_sz = sizeof ( ev.m_when ) + sizeof ( ev.eStatus ) + sizeof ( ev.adwMemTag ) + sizeof ( *ev.pnSuppInfo ) + sizeof ( ev.dwSeqNo );
    } else {
        buff_sz = sizeof ( ev.m_when ) + sizeof ( ev.eStatus ) + sizeof ( ev.adwMemTag ) + sizeof ( ev.dwSeqNo );
    }

    LOGD ( "the size of the buff of , NDsrcMemWriteEvent = %d", buff_sz );
    BYTE * buff = new BYTE [buff_sz];
    /*if(!ev.Pack( buff, buff_sz,&packed_sz)){
     *   LOGE("OnDsrcMemWriteEvent:the size of the buff is not long enough! ");
     *  return;
     *}
     */

    //Do want the member fucntion Pack do cause it miss "const" in the end of  its prototype
    INT nOffset = 0;
    memcpy ( &buff[ nOffset], &ev.m_when, sizeof ( ev.m_when ) );
    nOffset += sizeof ( ev.m_when );
    memcpy ( &buff[ nOffset], &ev.eStatus, sizeof ( ev.eStatus ) );
    nOffset += sizeof ( ev.eStatus );
    memcpy ( &buff[ nOffset], &ev.adwMemTag, sizeof ( ev.adwMemTag ) );
    nOffset += sizeof ( ev.adwMemTag );
    memcpy ( &buff[ nOffset], &ev.dwSeqNo, sizeof ( ev.dwSeqNo ) );
    nOffset += sizeof ( ev.dwSeqNo );

    if ( NDSRC_MEM_SUCCESS != ev.eStatus ) {
        memcpy ( &buff[ nOffset], &ev.pnSuppInfo->bDataSize, sizeof ( ev.pnSuppInfo->bDataSize ) );
        nOffset += sizeof ( ev.pnSuppInfo->bDataSize );
        memcpy ( &buff[ nOffset], ev.pnSuppInfo->abDataBody, ev.pnSuppInfo->bDataSize );
        nOffset += ev.pnSuppInfo->bDataSize;
    }

    jobject jParcel = createJavaParcelObject ( env );

    if ( jParcel != NULL ) {
        Parcel* nativeParcel = parcelForJavaObject ( env, jParcel );

        if ( buff != NULL  ) {
            Parcel * tempParcel = new Parcel;
            tempParcel->setData ( buff, buff_sz );
            delete buff;
            LOGD ( "OnDsrcMemWriteEvent::tempParcel.->dataSize() =%d", tempParcel->dataSize() );
            nativeParcel->writeInt32 ( tempParcel->dataSize() );
            nativeParcel->write ( tempParcel->data(), tempParcel->dataSize() );
            delete tempParcel;
            nativeParcel->setDataPosition ( 0 );
        } else {
            LOGE ( "OnDsrcMemWriteEvent::NO DATA!" );
            return;
        }

        LOGD ( "OnDsrcMemWriteEvent::nativeParcel->dataSize() =%d", nativeParcel->dataSize() );
        env->CallStaticVoidMethod ( fields.mClass, fields.mPost_event, mObjectWeakRef,
                                    eventId, 0, 0, jParcel );
        env->DeleteLocalRef ( jParcel );
    } else {
        LOGE ( "OnDsrcMemWriteEvent::create JavaParcel failed" );
        return;
    }
}
void DsrcJniListener::OnDsrcMemMibEvent ( const NDsrcMemMibEvent& ev ) {
    int eventId = ev.GetEventID();
    LOGD ( "OnDsrcMemMibEvent, eventID = %d", eventId );

    Mutex::Autolock l ( gLock );
    JNIEnv *env = AndroidRuntime::getJNIEnv();

    if ( NULL == env ) {
        LOGE ( "OnDsrcMemMibEvent:: can not find env!" );
    }

    //Do want the member fucntion Pack do cause it miss "const" in the end of  its prototype
    unsigned int buff_sz = sizeof ( ev.m_when ) + sizeof ( ev.eStatus ) + sizeof ( ev.nMibData ) + sizeof ( ev.dwSeqNo );
    LOGD ( "the size of the buff of OnDsrcMemMibEvent = %d", buff_sz );
    BYTE * buff = new BYTE [buff_sz];

    INT nOffset = 0;
    memcpy ( &buff[ nOffset], &ev.m_when, sizeof ( ev.m_when ) );
    nOffset += sizeof ( ev.m_when );
    memcpy ( &buff[ nOffset], &ev.eStatus, sizeof ( ev.eStatus ) );
    nOffset += sizeof ( ev.eStatus );
    memcpy ( &buff[ nOffset], &ev.nMibData, sizeof ( ev.nMibData ) );
    nOffset += sizeof ( ev.nMibData );
    memcpy ( &buff[ nOffset], &ev.dwSeqNo, sizeof ( ev.dwSeqNo ) );
    nOffset += sizeof ( ev.dwSeqNo );

    jobject jParcel = createJavaParcelObject ( env );

    if ( jParcel != NULL ) {
        Parcel* nativeParcel = parcelForJavaObject ( env, jParcel );

        if ( buff != NULL  ) {
            Parcel * tempParcel = new Parcel;
            tempParcel->setData ( buff, buff_sz );
            delete buff;
            LOGD ( "OnDsrcMemMibEvent::tempParcel.->dataSize() =%d", tempParcel->dataSize() );
            nativeParcel->writeInt32 ( tempParcel->dataSize() );
            nativeParcel->write ( tempParcel->data(), tempParcel->dataSize() );
            delete tempParcel;
            nativeParcel->setDataPosition ( 0 );
        } else {
            LOGE ( "OnDsrcMemMibEvent::NO DATA!" );
            return;
        }

        LOGD ( "OnDsrcMemMibEvent::nativeParcel->dataSize() =%d", nativeParcel->dataSize() );
        env->CallStaticVoidMethod ( fields.mClass, fields.mPost_event, mObjectWeakRef,
                                    eventId, 0, 0, jParcel );
        env->DeleteLocalRef ( jParcel );
    } else {
        LOGE ( "OnDsrcMemMibEvent::create JavaParcel failed" );
        return;
    }
}

static sp<DsrcJniListener> mDsrcJniListener = NULL;

static void jp_pioneer_ceam_handler_DSRCSystem_native_init ( JNIEnv *env ) {
    LOGD ( "JNI::native_init\n" );

    fields.mClass = env->FindClass ( kClassNPDsrcSystem );

    if ( fields.mClass == NULL ) {
        LOGE ( "Failed to get class reference" );
        return ;
    } else {
        fields.mClass = ( jclass ) env->NewGlobalRef ( fields.mClass );

        if ( fields.mClass == NULL ) {
            LOGE ( "Fail to get GlobalRef of jclass" );
        } else if ( fields.mPost_event == NULL ) {
            fields.mPost_event = env->GetStaticMethodID ( fields.mClass,
                                 "postEventFromNative",
                                 "(Ljava/lang/Object;IIILjava/lang/Object;)V" );

            if ( fields.mPost_event == NULL ) {
                LOGE ( "failed to get method id for postEventFromNative" );
                return;
            }
        }

        jclass mNDsrcClient = env->FindClass ( kClassNDsrcClient );

        if ( mNDsrcClient == NULL ) {
            LOGE ( "Failed to get class reference of NDsrcClient " );
            return ;
        } else {
            fields.dwSeq = env->GetFieldID ( mNDsrcClient, "dwSeq", "J" );

            if ( fields.dwSeq == NULL ) {
                LOGE ( "failed to get fielld ID of dwSeq in NDsrcClient" );
            }
        }
    }
}

static void jp_pioneer_ceam_handler_DSRCSystem_native_setup ( JNIEnv * env, jobject thiz, jobject weak_this ) {
    LOGD ( "JNI::Native_setup\n" );
    NPDsrcSystem  cNPDsrcSystem;

    if ( mDsrcJniListener == NULL ) {
        /*attach sysd*/
        SysdClientInfo info;
        info.setHandleEvent ( EV_DSRC_SEND_RESULT );
        info.setHandleEvent ( EV_DSRC_MEMORY_ALLOC );
        info.setHandleEvent ( EV_DSRC_MEMORY_WRITE );
        info.setHandleEvent ( EV_DSRC_MEMORY_ACCESS_MIB );

        //set all the other event ID in case they are notified from the Handler
        info.setHandleEvent ( EV_DSRC_STATUS );
        info.setHandleEvent ( EV_DSRC_PUSH );
        info.setHandleEvent ( EV_DSRC_RE_PUSH );
        info.setHandleEvent ( EV_DSRC_PUSH_ABORT );
        info.setHandleEvent ( EV_DSRC_FORWORDINFO_INIT_RES );
        info.setHandleEvent ( EV_DSRC_MEMORY_ACCESS_NOTIFY );
        info.setHandleEvent ( EV_DSRC_LPCP_CONNECT );
        info.setHandleEvent ( EV_DSRC_LPP_SPF_FINISH );
        info.setHandleEvent ( EV_DSRC_UNIT_INFO );
        info.setHandleEvent ( EV_DSRC_PPPCP_CONNECT );
        info.setHandleEvent ( EV_DSRC_SMARTPULL );
        info.setHandleEvent ( EV_DSRC_INDICATION );
        info.setHandleEvent ( EV_DSRC_CONFIRM );
        info.setHandleEvent ( EV_DSRC_INDICATION_RESULT );


        mDsrcJniListener = new DsrcJniListener ( thiz, weak_this );

        if ( mDsrcJniListener == NULL ) {
            LOGE ( "failed to get a new DsrcJniListener" );
            return;
        }

        mDsrcJniListener->attachSysd ( info );
    }
}


static jboolean jp_pioneer_ceam_handler_DSRCSystem_GetCurrentEvent ( JNIEnv * env, jobject thiz, jobject pnEvent ) {
    LOGD ( "JNI::GetCurrentEvent\n" );
    return false;
}

static jboolean jp_pioneer_ceam_handler_DSRCSystem_SendPushResponse ( JNIEnv * env, jobject thiz, jint bPushId,
        jbyteArray pbDataBody, jlong dwSeqNo, jint wPortNo, jint wHandleNo ) {
    LOGD ( "JNI::SendPushResponse\n" );

    //make a NDsrcPushResponse for native function
    NDsrcPushResponse mPushResponse;
    mPushResponse.bPushId = bPushId;

    if ( NULL != pbDataBody ) {
        mPushResponse.pnAcknowledge = new NDsrcPushAcknowledgement;
        mPushResponse.pnAcknowledge->dwDataSize = env->GetArrayLength ( pbDataBody );
        mPushResponse.pnAcknowledge->pbDataBody = ( BYTE * ) env->GetByteArrayElements ( pbDataBody, NULL );
    } else {
        LOGD ( "JNI::SendPushResponse::pbDataBody is NULL" );
    }

    NPDsrcSystem mNPDsrcSystem;
    jboolean result = mNPDsrcSystem.SendPushResponse ( &mPushResponse, dwSeqNo, wPortNo, wHandleNo );

    if ( NULL != mPushResponse.pnAcknowledge->pbDataBody ) {
        env->ReleaseByteArrayElements ( pbDataBody, ( jbyte* ) mPushResponse.pnAcknowledge->pbDataBody, 0 );
        delete mPushResponse.pnAcknowledge;
    }
	
    return result;
}


static jboolean jp_pioneer_ceam_handler_DSRCSystem_SendPushRerunResponse ( JNIEnv * env, jobject thiz, jint bPushId,
        jbyteArray pbDataBody, jlong dwSeqNo, jint wPortNo, jint wHandleNo ) {
    LOGD ( "JNI::SendPushRerunResponse\n" );

    //make a NDsrcPushResponse for native function
    NDsrcPushResponse mPushResponse;
    mPushResponse.bPushId = bPushId;

    if ( NULL != pbDataBody ) {
        mPushResponse.pnAcknowledge = new NDsrcPushAcknowledgement;
        mPushResponse.pnAcknowledge->dwDataSize = env->GetArrayLength ( pbDataBody );
        mPushResponse.pnAcknowledge->pbDataBody = ( BYTE * ) env->GetByteArrayElements ( pbDataBody, NULL );
    } else {
        LOGD ( "JNI::SendPushRerunResponse::pbDataBody is NULL" );
    }

    NPDsrcSystem mNPDsrcSystem;
    jboolean result = mNPDsrcSystem.SendPushRerunResponse ( &mPushResponse, dwSeqNo, wPortNo, wHandleNo );

    if ( NULL != mPushResponse.pnAcknowledge->pbDataBody ) {
        env->ReleaseByteArrayElements ( pbDataBody, ( jbyte* ) mPushResponse.pnAcknowledge->pbDataBody, 0 );
        delete mPushResponse.pnAcknowledge;
    }

    return result;
}


static jboolean jp_pioneer_ceam_handler_DSRCSystem_AbortPush ( JNIEnv * env, jobject thiz, jint bPushId, jint eStatus,
        jbyteArray abDataBody, jlong dwSeqNo, jint wPortNo , jint wHandleNo ) {
    LOGD ( "JNI::AbortPush\n" );
    //make a NDsrcPushAbort for native function
    NDsrcPushAbort pnPushAbort;
    pnPushAbort.bPushId = bPushId;

    if ( ( eStatus >= 0 && eStatus < 12 ) || eStatus == 0xFF ) {
        pnPushAbort.eStatus = ( NDsrcPushAbortStatus ) eStatus;
    } else {
        LOGE ( "JNI::AbortPush::Wrong parameter,out of the scope of enum\n" );
        return false;
    }

    if ( NULL != abDataBody ) {
        pnPushAbort.pnSuppInfo = new NDsrcSupplementInfo;
        pnPushAbort.pnSuppInfo->bDataSize = env->GetArrayLength ( abDataBody );
        BYTE * tempByteArray;
        tempByteArray = ( BYTE * ) ( env->GetByteArrayElements ( abDataBody, NULL ) );
        pnPushAbort.pnSuppInfo->abDataBody;

        for ( int i = 0; i < NDSRC_SUPPLEMENT_INFO_SIZE; i++ ) {
            pnPushAbort.pnSuppInfo->abDataBody[i] = tempByteArray[i];
        }

        env->ReleaseByteArrayElements ( abDataBody, ( jbyte* ) tempByteArray, 0 );
    } else {
        LOGD ( "JNI::AbortPush::pbDataBody is NULL" );
    }

    NPDsrcSystem mNPDsrcSystem;
    jboolean result = mNPDsrcSystem.AbortPush ( &pnPushAbort, dwSeqNo, wPortNo, wHandleNo );

    if ( NULL != pnPushAbort.pnSuppInfo ) {
        delete pnPushAbort.pnSuppInfo;
    }

    return result;
}

static jboolean jp_pioneer_ceam_handler_DSRCSystem_AllocateMemory ( JNIEnv * env, jobject thiz, jlongArray adwMemTag,
        jobject jMemAllocInfo, jbyteArray pbDataBody, jlong dwSeqNo ) {
    LOGD ( "JNI::AllocateMemory\n" );

    //make a NDsrcMemAllocInfo for native function
    NDsrcMemAllocInfo pnMemAllocInfo;
    jlong *tempLongArray = env->GetLongArrayElements ( adwMemTag, NULL );
    pnMemAllocInfo.adwMemTag[0] = tempLongArray[0];
    pnMemAllocInfo.adwMemTag[1] = tempLongArray[1];
    env->ReleaseLongArrayElements ( adwMemTag, tempLongArray, 0 );

    jclass jNDsrcMemAllocInfo = env->GetObjectClass ( jMemAllocInfo );

    if ( NULL == jNDsrcMemAllocInfo ) {
        LOGE ( "JNI::AllocateMemory::can not find class NDsrcMemAllocInfo" );
        return false;
    }

    jfieldID readProtectID = env->GetFieldID ( jNDsrcMemAllocInfo, "blReadProtect", "Z" );

    if ( NULL == readProtectID ) {
        LOGE ( "JNI::AllocateMemory::can not find FieldID of blReadProtect" );
        return false;
    }

    pnMemAllocInfo.blReadProtect = env->GetBooleanField ( jMemAllocInfo, readProtectID );

    jfieldID blSpfID = env->GetFieldID ( jNDsrcMemAllocInfo, "blSpf", "Z" );

    if ( NULL == blSpfID ) {
        LOGE ( "JNI::AllocateMemory::can not find FieldID of blSpf" );
        return false;
    }

    pnMemAllocInfo.blSpf = env->GetBooleanField ( jMemAllocInfo, blSpfID );

    jfieldID blWriteProtectID = env->GetFieldID ( jNDsrcMemAllocInfo, "blWriteProtect", "Z" );

    if ( NULL == blWriteProtectID ) {
        LOGE ( "JNI::AllocateMemory::can not find FieldID of blWriteProtect" );
        return false;
    }

    pnMemAllocInfo.blWriteProtect = env->GetBooleanField ( jMemAllocInfo, blWriteProtectID );

    jfieldID dwMaxMemorySizeID = env->GetFieldID ( jNDsrcMemAllocInfo, "dwMaxMemorySize", "J" );

    if ( NULL == dwMaxMemorySizeID ) {
        LOGE ( "JNI::AllocateMemory::can not find FieldID of dwMaxMemorySize" );
        return false;
    }

    pnMemAllocInfo.dwMaxMemorySize = env->GetBooleanField ( jMemAllocInfo, dwMaxMemorySizeID );
    pnMemAllocInfo.dwDataSize = env->GetArrayLength ( adwMemTag );

    if ( NULL != pbDataBody ) {
        pnMemAllocInfo.pbDataBody = ( BYTE* ) env->GetByteArrayElements ( pbDataBody, NULL );
    } else {
        LOGD ( "JNI::AllocateMemory::pbDataBody is NULL" );
    }

    NPDsrcSystem mNPDsrcSystem;
    jboolean result = mNPDsrcSystem.AllocateMemory ( &pnMemAllocInfo, dwSeqNo );

    if ( NULL != pnMemAllocInfo.pbDataBody ) {
        env->ReleaseByteArrayElements ( pbDataBody, ( jbyte* ) pnMemAllocInfo.pbDataBody, 0 );
    }

    return result;
}

static jboolean jp_pioneer_ceam_handler_DSRCSystem_WriteMemory ( JNIEnv * env, jobject thiz, jlongArray adwMemTag,
        jbyteArray pbDataBody, jlong dwSeqNo ) {
    LOGD ( "JNI::WriteMemory\n" );

    //make a NDsrcMemData for native function
    NDsrcMemData pnMemData;
    jlong *tempLongArray = env->GetLongArrayElements ( adwMemTag, NULL );
    pnMemData.adwMemTag[0] = tempLongArray[0];
    pnMemData.adwMemTag[1] = tempLongArray[1];
    env->ReleaseLongArrayElements ( adwMemTag, tempLongArray, 0 );

    if ( NULL != pbDataBody ) {
        pnMemData.dwDataSize = env->GetArrayLength ( pbDataBody );
        pnMemData.pbDataBody = ( BYTE* ) env->GetByteArrayElements ( pbDataBody, NULL );
    }

    NPDsrcSystem mNPDsrcSystem;
    jboolean result = mNPDsrcSystem.WriteMemory ( &pnMemData, dwSeqNo );

    if ( NULL != pnMemData.pbDataBody ) {
        env->ReleaseByteArrayElements ( pbDataBody, ( jbyte* ) pnMemData.pbDataBody, 0 );
    }

    return result;
}

static jboolean jp_pioneer_ceam_handler_DSRCSystem_SetMemoryMibData ( JNIEnv * env, jobject thiz,
        jobject jDsrcMemMibData, jlong dwSeqNo ) {
    LOGD ( "JNI::SetMemoryMibData\n" );

    //use the data from the java level to build the paramters
    NDsrcMemMibData  cDsrcMemMibData;
    jclass MemMibDataIClazz = env->GetObjectClass ( jDsrcMemMibData );

    if ( MemMibDataIClazz == NULL ) {
        LOGE ( "can not find class DsrcMemMibData " );
        return false;
    }

    jfieldID NDsrcMemMibIdFieldID = env->GetFieldID ( MemMibDataIClazz, "eMibId", "I" );

    if ( NDsrcMemMibIdFieldID == NULL ) {
        LOGE ( "can not find field ID of  NDsrcMemMib " );
        return false;
    }

    int tempEMibId = ( NDsrcMemMibId ) env->GetIntField ( jDsrcMemMibData, NDsrcMemMibIdFieldID );

    if ( tempEMibId < 0 || tempEMibId > 2 ) {
        LOGE ( "WRONG Parameter::NDsrcMemMibData" );
        return false;
    } else {
        cDsrcMemMibData.eMibId = ( NDsrcMemMibId ) tempEMibId;
    }

    jfieldID dwMibParamFieldID = env->GetFieldID ( MemMibDataIClazz, "dwMibParam", "J" );

    if ( dwMibParamFieldID == NULL ) {
        LOGE ( "can not find field ID of  dwMibParam " );
        return false;
    }

    cDsrcMemMibData.dwMibParam = env->GetIntField ( jDsrcMemMibData, dwMibParamFieldID );

    //call the local function to set the memory
    NPDsrcSystem  mNPDsrcSystem;
    jboolean ret = mNPDsrcSystem.SetMemoryMibData ( &cDsrcMemMibData, dwSeqNo );

    return ret;
}


static jboolean jp_pioneer_ceam_handler_DSRCSystem_StartLogging ( JNIEnv * env, jobject thiz, jstring pszDirName ) {
    LOGD ( "JNI::StartLogging\n" );

    if ( pszDirName == NULL ) {
        LOGE ( "the path name can not be NULL" );
        return NP_ERROR_PARAMETER;
    }

    const char *str ;
    str = env->GetStringUTFChars ( pszDirName, NULL );

    if ( str == NULL ) {
        LOGE ( "failed to getStringUTFChars,out of memory" );
        return false;
    }

    //make a new copy of str because the API function can not take const char * as argument
    int length = strlen ( str );
    char* tempStr = new char [length + 1];
    strcpy ( tempStr, str );
    env->ReleaseStringUTFChars ( pszDirName, str );

    NPDsrcSystem mNPDsrcSystem;
    jboolean result = mNPDsrcSystem.StartLogging ( tempStr );

    delete tempStr;
    return result;
}

static jboolean jp_pioneer_ceam_handler_DSRCSystem_StopLogging ( JNIEnv * env, jobject thiz ) {
    LOGD ( "JNI::StopLogging\n" );
    NPDsrcSystem  cNPDsrcSystem;
    return cNPDsrcSystem.StopLogging();
}


static JNINativeMethod gMethods[] = {
    {
        "native_init",
        "()V",
        ( void * ) jp_pioneer_ceam_handler_DSRCSystem_native_init
    },
    {
        "native_setup",
        "(Ljava/lang/Object;)V",
        ( void * ) jp_pioneer_ceam_handler_DSRCSystem_native_setup
    },
    {
        "GetCurrentEvent",
        "(Ljp/pioneer/ceam/handler/NPDsrcSystem$NDsrcEvent;)Z",
        ( void * ) jp_pioneer_ceam_handler_DSRCSystem_GetCurrentEvent
    },
    {
        "SendPushResponse",
        "(B[BJII)Z",
        ( void * ) jp_pioneer_ceam_handler_DSRCSystem_SendPushResponse
    },
    {
        "SendPushRerunResponse",
        "(B[BJII)Z",
        ( void * ) jp_pioneer_ceam_handler_DSRCSystem_SendPushRerunResponse
    },
    {
        "AbortPush",
        "(BI[BJII)Z",
        ( void * ) jp_pioneer_ceam_handler_DSRCSystem_AbortPush
    },
    {
        "AllocateMemory",
        "([JLjp/pioneer/ceam/handler/NPDsrcSystem$NDsrcMemAllocInfo;[BJ)Z",
        ( void * ) jp_pioneer_ceam_handler_DSRCSystem_AllocateMemory
    },
    {
        "WriteMemory",
        "([J[BJ)Z",
        ( void * ) jp_pioneer_ceam_handler_DSRCSystem_WriteMemory
    },
    {
        "SetMemoryMibData",
        "(Ljp/pioneer/ceam/handler/NPDsrcSystem$NDsrcMemMibData;J)Z",
        ( void * ) jp_pioneer_ceam_handler_DSRCSystem_SetMemoryMibData
    },
    {
        "StartLogging",
        "(Ljava/lang/String;)Z",
        ( void * ) jp_pioneer_ceam_handler_DSRCSystem_StartLogging
    },
    {
        "StopLogging",
        "()Z",
        ( void * ) jp_pioneer_ceam_handler_DSRCSystem_StopLogging
    }
};

static int register_jp_poineer_ceam_handler_DSRCSystem ( JNIEnv * env ) {
    LOGD ( "JNI register_jp_poineer_ceam_handler_DSRCSystem\n" );
    return AndroidRuntime::registerNativeMethods ( env, kClassNPDsrcSystem, gMethods, NELEM ( gMethods ) );

}

extern "C" jint JNI_OnLoad ( JavaVM * vm, void * reserved ) {
    JNIEnv* env = NULL;
    jint result = -1;

    if ( vm->GetEnv ( ( void** ) &env, JNI_VERSION_1_4 ) != JNI_OK ) {
        LOGE ( "ERROR: GetEnv failed\n" );
        return result;
    }

    assert ( env != NULL );

    if ( register_jp_poineer_ceam_handler_DSRCSystem ( env ) < 0 ) {
        LOGE ( "ERROR: DSRCSystem native registration failed\n" );
        return result;
    } else {
        LOGD ( "DSRCSystem native registration successed\n" );
        result = JNI_VERSION_1_4;
    }

    return result;
}

