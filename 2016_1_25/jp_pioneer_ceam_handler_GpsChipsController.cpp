/*
* (c) PIONEER CORPORATION 2014
* 25-1 Nishi-machi Yamada Kawagoe-shi Saitama-ken 350-8555 Japan
* All Rights Reserved.
*/

#define LOG_TAG "GpsChipsController-JNI"
#include "log_id.h"
#include "frameworksErrorCode.def"
#define LOG_ID LOGID_GPSCHIPS

#include <utils/Log.h>
#include <jni.h>
#include <JNIHelp.h>
#include <android_runtime/AndroidRuntime.h>
#include <binder/Parcel.h>

#include "android_os_Parcel.h"
#include "GpsChipsServerListener.h"
#include "GpsChipsServerDefines.h"
#include "GpsChipsControllerDef.h"
#include "GpsChipsClient.h"

#define DEBUG_NOTIFY 0

namespace android
{
/*change the values (start with "FUNCTION_")below along
 *with there conterpart in java level
 */
enum GET_FUNCTION_ID {
    FUNCTION_GET_SENSOR_INFO,
    FUNCTION_GET_GSNSAD4DRIANA,
    FUNCTION_GET_GetGNssValidityDate
};

enum SET_FUNCTION_ID {
    FUNCTION_SET_MAP_MATCH_INFO = 10,
    FUNCTION_SET_SENSOR_DIRECTION
};

static const char* const kClassGpsChipsController =
    "jp/pioneer/ceam/handler/GpsChipsController";

//Mutex sLock;

struct fields_t {
    jclass      mClass;
    jfieldID    mNaitveContext;
    jmethodID   mPost_event;
};

struct fields_t fields = {NULL, NULL, NULL};

class MyJNIListener: public GpsChipsServerListener
{
private:
    jobject mObjRef;//weakreference of controllers

public:
    MyJNIListener( jobject ref )
        : GpsChipsServerListener( GCS_LISTENER_TYPE_LOCMAILDATA ),
          mObjRef( ref ) {
        LOGD( "new MyJNIListener" );
    }

    virtual ~MyJNIListener() {
        LOGD( "~MyJNIListener" );
    }

    virtual void notify( int id, const Parcel* obj ) {
        if ( DEBUG_NOTIFY ) {
            LOGD( "jni->notify::id =%d", id );
        }

        //call postEventFromNative
        if ( id == GC_DATA_NOTIFY_ID_LOCMAILDATA ) {
            JNIEnv* env = AndroidRuntime::getJNIEnv();

            if ( DEBUG_NOTIFY ) {
                LOGD( "notify  obj->dataSize() =%d", obj->dataSize() );
            }

            if ( obj && ( obj->dataSize() > 0 ) ) {
                jobject jParcel = createJavaParcelObject( env );

                if ( jParcel != NULL ) {
                    if ( DEBUG_NOTIFY ) {
                        LOGD( "notify::create JavaParcel successed" );
                    }

                    Parcel* nativeParcel = parcelForJavaObject( env, jParcel );
                    /*Write the length of the byte array at the beginning of the parcel
                     *for the use of createByteArray() int java level
                     */
                    nativeParcel->writeInt32( obj->dataSize() );
                    nativeParcel->write( obj->data(), obj->dataSize() );
                    //reset the parcel's data position for further read
                    nativeParcel->setDataPosition( 0 );

                    if ( DEBUG_NOTIFY ) {
                        LOGD( "notify  nativeParcel->dataSize() =%d", nativeParcel->dataSize() );
                    }

                    env->CallStaticVoidMethod( fields.mClass, fields.mPost_event, mObjRef,
                                               GC_DATA_NOTIFY_ID_LOCMAILDATA, 0, 0, jParcel );
                    env->DeleteLocalRef( jParcel );
                } else {
                    LOGERROR( ERRCODE_GPSCHIPS_PARAM_ERROR, "notify::create JavaParcel failed" );
                    return;
                }
            } else {
                env->CallStaticVoidMethod( fields.mClass, fields.mPost_event, mObjRef,
                                           GC_DATA_NOTIFY_ID_LOCMAILDATA, 0, 0, NULL );
            }
        }
    }
};

static sp<GpsChipsClient> gClient = NULL;
static sp<MyJNIListener> gListener = NULL;

class GpsServerConnectThread: public Thread
{
public:
    GpsServerConnectThread( sp<GpsChipsClient>& chipsClient, jobject ref )
        : mChipsClient( chipsClient ),
          mObjRef( ref ) {
        LOGD( "new GpsServerConnectThread" );
    }

private:
    virtual bool        threadLoop() {
        LOGD( "threadLoop is called" );

        if ( mChipsClient->attachGpsChipsServer() ) {
            //if server not ready,wait a moment
            usleep( 500 );
            return true;
        }

        gListener = new MyJNIListener( mObjRef );
        gListener->registerListener();
        LOGD( "regist listener success" );
        return false;
    }

private:
    sp<GpsChipsClient>& mChipsClient;
    jobject mObjRef;
};

static bool init( JNIEnv* env, jobject thiz )
{
    LOGD( "JNI init" );

    if ( fields.mClass == NULL ) {
        fields.mClass = env->GetObjectClass( thiz );

        if ( fields.mClass == NULL ) {
            LOGE( "get class reference failed" );
            return false;
        } else {
            LOGD( "get class reference success" );
            fields.mClass = ( jclass ) env->NewGlobalRef( fields.mClass );
        }
    }

#if 0

    if ( fields.mNaitveContext == NULL ) {
        fields.mNaitveContext = env->GetFieldID( fields.mClass,
                                                 "mNaitveContext",
                                                 "I" );

        if ( fields.mNaitveContext == NULL ) {
            LOGE( "can not find filedid for mNaitveContext" );
            return false;
        } else {
            LOGD( "get field id success" );
        }
    }

#endif

    if ( fields.mPost_event == NULL ) {
        fields.mPost_event = env->GetStaticMethodID( fields.mClass,
                                                     "postEventFromNative",
                                                     "(Ljava/lang/Object;IIILjava/lang/Object;)V" );

        if ( fields.mPost_event == NULL ) {
            LOGE( "get method id for postEventFromNative failed" );
            return false;
        } else {
            LOGD( "get method id for postEventFromNative success" );
        }
    }

    return true;
}

static void jp_pioneer_ceam_GpsChipsController_native_setup(
    JNIEnv* env, jobject thiz, jobject objRef )
{
    LOGD( "jni->native_setup" );

    if ( !init( env, thiz ) ) {
        LOGE( "init failed" );
    }

    gClient = new GpsChipsClient();

    if ( gClient == NULL ) {
        LOGE( "new GpsChipsClient failed" );
        return;
    } else {
        // attach GpsChipsServer
        jobject ref = env->NewGlobalRef( objRef );  // when to delete????
        sp<GpsServerConnectThread> thread = new GpsServerConnectThread( gClient, ref );
        thread->run( "GpsServerConnectThread" );
        LOGE( "run GpsServerConnectThread successed" );
    }
}

//write the data in the GC_SensorInfo to the parcel
static void writeGyroSensorInfo( Parcel* mParcel, const GC_GyroSensorInfo mGyroSensorInfo )
{
    LOGD( "jni->writeGyroSensorInfo" );
    mParcel->writeFloat( mGyroSensorInfo.fADValue );
    mParcel->writeFloat( mGyroSensorInfo.fDispersionValue );
    mParcel->writeInt32( mGyroSensorInfo.bAngularVelocityOk );
    mParcel->writeFloat( mGyroSensorInfo.fAngularVelocity );
    mParcel->writeInt32( mGyroSensorInfo.byOutputValue );
    mParcel->writeInt32( mGyroSensorInfo.byAbnormalStatus );
}

static void writeAccSensorInfo( Parcel* mParcel, const GC_AccSensorInfo mAccSensorInf )
{
    LOGD( "jni->writeAccSensorInfo" );
    mParcel->writeFloat( mAccSensorInf.fADValue );
    mParcel->writeFloat( mAccSensorInf.fDispersionValue );
    mParcel->writeInt32( mAccSensorInf.bAccelerationOk );
    mParcel->writeFloat( mAccSensorInf.fAcceleration );
    mParcel->writeInt32( mAccSensorInf.byOutputValue );
    mParcel->writeInt32( mAccSensorInf.byAbnormalStatus );
}

static void writeSensoInfo( Parcel* mParcel, const GC_SensorInfo& mSensorInfo )
{
    LOGD( "jni->writeSensoInfo" );
    mParcel->writeInt32( mSensorInfo.bySensorInfoFormat );
    mParcel->writeInt64( mSensorInfo.ulTimeStamp );
    mParcel->writeInt64( mSensorInfo.ulDeviceSamplingEndTime );
    mParcel->writeInt32( mSensorInfo.bySensorLearnMode );
    mParcel->writeInt32( mSensorInfo.byTripStatus );
    mParcel->writeInt32( mSensorInfo.speedLearnCorr );
    mParcel->writeFloat( mSensorInfo.speedLearnRate );
    mParcel->writeInt32( mSensorInfo.usSpeedLearnDegree );
    mParcel->writeInt32( mSensorInfo.usGyroLearnDegreeRight );
    mParcel->writeInt32( mSensorInfo.usGyroLearnDegreeLeft );
    mParcel->writeInt32( mSensorInfo.usGsnsLearnDegreeRight );
    mParcel->writeInt32( mSensorInfo.usGsnsLearnDegreeLeft );
    mParcel->writeInt32( mSensorInfo.usRollLearnDegreeRight );
    mParcel->writeInt32( mSensorInfo.usRollLearnDegreeLeft );
    mParcel->writeInt32( mSensorInfo.usDistLearnDegree );
    mParcel->writeInt32( mSensorInfo.usDirLearnDegreeRight );
    mParcel->writeInt32( mSensorInfo.usDirLearnDegreeLeft );
    mParcel->writeInt32( mSensorInfo.us3DLearnDegree );
    mParcel->writeDouble( mSensorInfo.dOdoMeter );
    mParcel->writeDouble( mSensorInfo.dOdoMeterNoReset );
    mParcel->writeInt32( mSensorInfo.bSpeedOk );
    mParcel->writeFloat( mSensorInfo.fSpeed );
    mParcel->writeInt32( mSensorInfo.bAccelerationOk );
    mParcel->writeFloat( mSensorInfo.fAcceleration );
    mParcel->writeInt32( mSensorInfo.bForwardBackOk );
    mParcel->writeInt32( mSensorInfo.byForwardBack );
    mParcel->writeInt32( mSensorInfo.bStopStatusOk );
    mParcel->writeInt32( mSensorInfo.byStopStatus );
    mParcel->writeInt32( mSensorInfo.bGyroOk );
    mParcel->writeFloat( mSensorInfo.fGyro );
    mParcel->writeInt32( mSensorInfo.bGsnsOk );
    mParcel->writeFloat( mSensorInfo.fGsns );
    mParcel->writeInt32( mSensorInfo.bRollAngleOk );
    mParcel->writeFloat( mSensorInfo.fRollAngle );

    for ( int i = 0; i < 3; i++ ) {
        writeGyroSensorInfo( mParcel, mSensorInfo.aGyroSensorInfo[i] );
    }

    for ( int i = 0; i < 3; i++ ) {
        writeAccSensorInfo( mParcel, mSensorInfo.aAccSensorInfo[i] );
    }

    mParcel->writeInt32( mSensorInfo.bSpeedPluseOk );
    mParcel->writeInt64( mSensorInfo.ulSpeedPulseSum );
    mParcel->writeInt32( mSensorInfo.usSpeedPulseNum );
    mParcel->writeInt32( mSensorInfo.byPluseAbnormalStatus );
    mParcel->writeInt32( mSensorInfo.bReverseConnectOk );
    mParcel->writeInt32( mSensorInfo.byReverseStatus );
    mParcel->writeInt32( mSensorInfo.byReverseAbnormalStatus );
    mParcel->writeFloat( mSensorInfo.fTempSensorADValue );
    mParcel->writeFloat( mSensorInfo.fTempSensorDispValue );
    mParcel->writeInt32( mSensorInfo.bTempOk );
    mParcel->writeFloat( mSensorInfo.fTemp );
    mParcel->writeInt32( mSensorInfo.byTempAbnormalStatus );
    mParcel->writeInt32( mSensorInfo.bYawOk );
    mParcel->writeFloat( mSensorInfo.fYaw );
    mParcel->writeInt32( mSensorInfo.bPitchOk );
    mParcel->writeFloat( mSensorInfo.fPitch );
    mParcel->writeInt32( mSensorInfo.bRollOk );
    mParcel->writeFloat( mSensorInfo.fRoll );
    mParcel->writeInt32( mSensorInfo.byVibrationStatus );
    mParcel->writeInt32( mSensorInfo.byAbnormalCounter );
    mParcel->writeInt32( mSensorInfo.byChangeCounter );
    mParcel->writeFloat( mSensorInfo.fMostLowSpeed );
    LOGD( "jni->writeSensoInfo END" );
}

static void writeGsnsAdData( Parcel* mParcel, const GC_GpsSnsGsnsAdStr& mGsnsAdData )
{
    LOGD( "jni->writeGsnsAdData" );
    mParcel->writeInt32( mGsnsAdData.byDataCount );
    mParcel->writeInt32( mGsnsAdData.byOverflowCount );
    mParcel->writeInt32( mGsnsAdData.byModelCode );

    for ( int i = 0; i < 135; i++ ) {
        mParcel->writeInt32( mGsnsAdData.sGsnsADData.byGsnsXyzAd[i] );
    }
}

static void writeGC_Time( Parcel* mParcel , const GC_Time& mTime )
{
    LOGD( "jni->writeGC_Time" );
    mParcel->writeInt32( mTime.usYear );
    mParcel->writeInt32( mTime.byMonth );
    mParcel->writeInt32( mTime.byDay );
    mParcel->writeInt32( mTime.byHour );
    mParcel->writeInt32( mTime.byMinute );
    mParcel->writeInt32( mTime.bySecond );
    mParcel->writeInt32( mTime.byReserved );
}



static void getMapMatchingStatusFromParcel( Parcel* mParcel, GC_MapMatchingStatus& m_MapMatchStatus )
{
    LOGD( "jni->getMapMatchingStatusFromParcel" );
    m_MapMatchStatus.ulReserveBits = mParcel->readInt64();
    m_MapMatchStatus.ulRapidCorrHeading = mParcel->readInt64();
    m_MapMatchStatus.ulResetHeadingFlag = mParcel->readInt64();
    m_MapMatchStatus.ulGpsReset = mParcel->readInt64();
    m_MapMatchStatus.ulMmPosOk = mParcel->readInt64();
    m_MapMatchStatus.ulRunStatus = mParcel->readInt64();
    m_MapMatchStatus.ulRoadRollAngleOK = mParcel->readInt64();
    m_MapMatchStatus.ulRoadSlopeAngleOK = mParcel->readInt64();
    m_MapMatchStatus.ulRoadAngleOk = mParcel->readInt64();
}

static void getMapMatchingRoadStatusFromParcel( Parcel* mParcel, GC_MapMatchingRoadStatus& mMapMatchRoadStatus )
{
    LOGD( "jni->getMapMatchingRoadStatusFromParcel" );
    mMapMatchRoadStatus.ulReserveBits = mParcel->readInt64();
    mMapMatchRoadStatus.ulIntercityWay = mParcel->readInt64();
    mMapMatchRoadStatus.ulUrbanFreeWay = mParcel->readInt64();
    mMapMatchRoadStatus.ulTunnel = mParcel->readInt64();
    mMapMatchRoadStatus.ulMountain = mParcel->readInt64();
    mMapMatchRoadStatus.ulUnderParking = mParcel->readInt64();
}

static void getMapMatchingDataFromParcel( Parcel* mParcel, GC_MapMatchingData& m_MapMatchData )
{
    LOGD( "jni->getMapMatchingDataFromParcel" );
    m_MapMatchData.byMapMatchInfoCounter = mParcel->readInt32();
    m_MapMatchData.byGeodesic =  mParcel->readInt32();
    m_MapMatchData.dLat = mParcel->readDouble();
    m_MapMatchData.dLon = mParcel->readDouble();
    m_MapMatchData.fHeight = mParcel->readFloat();
    getMapMatchingStatusFromParcel( mParcel, m_MapMatchData.sStatus );
    getMapMatchingRoadStatusFromParcel( mParcel, m_MapMatchData.sRoadStatus );
    m_MapMatchData.fRoadDir = mParcel->readFloat();
    m_MapMatchData.fRoadInclination = mParcel->readFloat();
    m_MapMatchData.fRoadRoll = mParcel->readFloat();
}



static jint  jp_pioneer_ceam_GpsChipsController_getDataFromJNI(
    JNIEnv* env, jobject thiz, jint id, jobject jParcel )
{
    LOGD( "jni->getDataFromJNI id: %d", id );
    jint ret = -1;
    Parcel* nativeParcel = parcelForJavaObject( env, jParcel );

    if ( nativeParcel == NULL ) {
        LOGD( "failed to get native parcel for java parcel" );
    } else {
        LOGD( "success to get native parcel for java parcel" );
    }

    nativeParcel->setDataPosition( 0 );

    switch ( id ) {
        case  FUNCTION_GET_SENSOR_INFO : {
            LOGD( "function getSensorInfo is called" );
            GC_SensorInfo mSensorInfo;
            ret = gClient->getSensorInfo( mSensorInfo );
            writeSensoInfo( nativeParcel, mSensorInfo );
            break;
        }

        case FUNCTION_GET_GSNSAD4DRIANA: {
            GC_GpsSnsGsnsAdStr mGsnsAdData;
            ret = gClient->gyroGetGsnsAD4DrivingAnalysis( mGsnsAdData );
            writeGsnsAdData( nativeParcel, mGsnsAdData );
            break;
        }

        case FUNCTION_GET_GetGNssValidityDate: {
            GC_Time sStartTime;
            GC_Time sEndTime;
            ret = gClient->getGnssValidityDate( sStartTime, sEndTime );
            LOGD( "sStartTime.usYear = %d", sStartTime.usYear );
            LOGD( "sStartTime.byReserved = %d ", sStartTime.byReserved );
            LOGD( "sEndTime.usYear = %d ", sEndTime.usYear );
            LOGD( "sEndTime.byReserved = %d ", sEndTime.byReserved );
            writeGC_Time( nativeParcel, sStartTime );
            writeGC_Time( nativeParcel, sEndTime );
            break;
        }
    }

    if ( ret == -1 ) {
        LOGD( "unknow fuction ID" );
    }

    return ret;
}

static jint jp_pioneer_ceam_GpsChipsController_setDataToJNI(
    JNIEnv* env, jobject thiz, jint id, jobject jParcel )
{
    LOGD( "jni->setDataToJNI id: %d", id );
    jint ret = -1;
    Parcel* nativeParcel = parcelForJavaObject( env, jParcel );
    nativeParcel->setDataPosition( 0 );

    switch ( id ) {
        case FUNCTION_SET_MAP_MATCH_INFO: {
            GC_MapMatchingData mapMatchingData;
            getMapMatchingDataFromParcel( nativeParcel, mapMatchingData );
            ret = gClient->setMapMatchingInfo( mapMatchingData );
            break;
        }
    }

    return ret;
}

static jint jp_pioneer_ceam_GpsChipsController_setSensorDirection(
    JNIEnv* env, jobject thiz, jfloat fDir )
{
    int ret = gClient->setSensorDirection( fDir );
    return ret;
}


static JNINativeMethod gMethods[] = {
    {
        "native_setup",
        "(Ljava/lang/Object;)V",
        ( void* ) jp_pioneer_ceam_GpsChipsController_native_setup
    },

    {
        "getDataFromJNI",
        "(ILandroid/os/Parcel;)I",
        ( void* ) jp_pioneer_ceam_GpsChipsController_getDataFromJNI
    },
    {
        "setDataToJNI",
        "(ILandroid/os/Parcel;)I",
        ( void* ) jp_pioneer_ceam_GpsChipsController_setDataToJNI
    },
    {
        "setSensorDirection",
        "(F)I",
        ( void* ) jp_pioneer_ceam_GpsChipsController_setSensorDirection
    }
};

static int register_jp_poineer_ceam_handler_GpsChipsController( JNIEnv* env )
{
    return AndroidRuntime::registerNativeMethods( env,
                                                  kClassGpsChipsController, gMethods, NELEM( gMethods ) );
}

extern "C" jint JNI_OnLoad( JavaVM* vm, void* reserved )
{
    JNIEnv* env = NULL;
    jint result = -1;

    if ( vm->GetEnv( ( void** ) &env, JNI_VERSION_1_4 ) != JNI_OK ) {
        LOGE( "ERROR: GetEnv failed\n" );
        return result;
    }

    LOGE( "GetEnv successed\n" );
    assert( env != NULL );

    if ( register_jp_poineer_ceam_handler_GpsChipsController( env ) < 0 ) {
        LOGE( "ERROR: GpsChipsServer native registration failed\n" );
        return result;
    } else {
        LOGE( "GpsChipsServer native registration successed\n" );
        result = JNI_VERSION_1_4;
    }

    return result;
}
}
