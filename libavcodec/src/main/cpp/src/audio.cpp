#include <assert.h>
#include <jni.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <android/log.h>

// 录制音频使用
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

// asset manager
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <sys/types.h>

#include "audio.h"
#include "RecordBuffer.h"

#define TAG "audio"

#define LOGD(TAG, ...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
// 定义info信息
#define LOGI(TAG, ...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)
// 定义warn信息
#define LOGW(TAG, ...) __android_log_print(ANDROID_LOG_WARN,TAG,__VA_ARGS__)
// 定义error信息
#define LOGE(TAG, ...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)



// opensles 采集步骤
// 1. 初始化引擎
// 2. 配置音频采集参数
// 3. 启动/停止采集
// 4. 释放资源




// engine interfaces
static SLObjectItf engineObject = NULL;
static SLEngineItf engineEngine;

// recorder interfaces
static SLObjectItf recorderObject = NULL;
static SLRecordItf recorderRecord;
static SLAndroidSimpleBufferQueueItf recorderBufferQueue;

RecordBuffer *buffer;

// 文件路径
char file_path[1024] = {0};

//PCM文件
FILE *pcmFile;

#define RECORDER_FRAMES (1024 * 2)

int recorder_size = RECORDER_FRAMES * 2;

static pthread_mutex_t audioEngineLock = PTHREAD_MUTEX_INITIALIZER;

// this callback handler is called every time a buffer finishes recording
void bqRecorderCallback(SLAndroidSimpleBufferQueueItf bq, void *context) {
    assert(bq == recorderBufferQueue);
    assert(NULL == context);
    // 写文件
    if (NULL != pcmFile){
        fwrite(buffer->getNowBuffer(), 1, recorder_size, pcmFile);
    }
    // 编码
    (*recorderBufferQueue)->Enqueue(recorderBufferQueue, buffer->getRecordBuffer(), recorder_size);
    pthread_mutex_unlock(&audioEngineLock);
}


/**
 * 创建OpenSLES
 * @return  0: success other: fail
 */
int create_engine() {

    SLresult result;

    buffer = new RecordBuffer(recorder_size);
    // 创建engine
    result = slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
    if (SL_RESULT_SUCCESS != result) {
        return -1;
    }

    // 初始化引擎
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    if (SL_RESULT_SUCCESS != result) {
        return -1;
    }

    // 获取接口
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    if (SL_RESULT_SUCCESS != result) {
        return -1;
    }

    return 0;
}


/**
 * 创建recorder
 * @param micId      mic id
 * @param sample     采样率
 * @param channel    通道数
 * @param frameLen   帧长
 * @return  0： success other：fail
 */
int create_recorder(int micId, int sample, int channel, int frameLen) {
    SLresult result;

    // 配置audio源
    SLDataLocator_IODevice loc_dev = {
            SL_DATALOCATOR_IODEVICE,            // 类型：IO设备类型，手机内置MIC
            SL_IODEVICE_AUDIOINPUT,              // 设备类型： 音频输入类型
            SL_DEFAULTDEVICEID_AUDIOINPUT,         // 设备ID
            NULL};                                   // device实例
    // 输入，SLdataSource， 表示音频数据来源信息
    SLDataSource audioSrc = {&loc_dev, NULL};

    // 缓冲区队列
    SLDataLocator_AndroidSimpleBufferQueue loc_bq = {
            SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,   // buffer类型
            2     // buffer的数量
    };
    // PCM 数据源格式 //设置输出数据的格式
    SLDataFormat_PCM format_pcm = {
            SL_DATAFORMAT_PCM,
            2,
            SL_SAMPLINGRATE_48,         // 采样率
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
            SL_BYTEORDER_LITTLEENDIAN};
    SLDataSink audioSnk = {&loc_bq, &format_pcm};

    const SLInterfaceID id[1] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE};
    const SLboolean req[1] = {SL_BOOLEAN_TRUE};
    result = (*engineEngine)
            ->CreateAudioRecorder(engineEngine, &recorderObject, &audioSrc,
                                  &audioSnk, 1, id, req);
    if (SL_RESULT_SUCCESS != result) {
        LOGE(TAG, "CreateAudioRecorder fail .error: %d", result);
        return -1;
    }

    // realize the audio recorder
    result = (*recorderObject)->Realize(recorderObject, SL_BOOLEAN_FALSE);
    if (SL_RESULT_SUCCESS != result) {
        LOGE(TAG, "CreateAudioRecorder realize the audio recorder fail .error: %d", result);
        return -1;
    }

    // get the record interface
    result = (*recorderObject)->GetInterface(recorderObject, SL_IID_RECORD, &recorderRecord);
    if (SL_RESULT_SUCCESS != result) {
        LOGE(TAG, "CreateAudioRecorder  get the record interface fail .error: %d", result);
        return -1;
    }

    // get the buffer queue interface
    result = (*recorderObject)->GetInterface(recorderObject, SL_IID_ANDROIDSIMPLEBUFFERQUEUE,
                                             &recorderBufferQueue);
    if (SL_RESULT_SUCCESS != result) {
        LOGE(TAG, "CreateAudioRecorder get the buffer queue interface  fail .error: %d", result);
        return -1;
    }

    // register callback on the buffer queue
    result = (*recorderBufferQueue)->RegisterCallback(recorderBufferQueue, bqRecorderCallback,
                                                      NULL);
    if (SL_RESULT_SUCCESS != result) {
        LOGE(TAG, "CreateAudioRecorder register callback on the buffer queue  fail .error: %d",
             result);
        return -1;
    }

    return 0;
}


/**
 * 开启采集
 * @return 0： success other： fail
 */
int start_recording() {
    LOGD(TAG, "start recording");
    SLresult result;
    if (pthread_mutex_trylock(&audioEngineLock)) {
        LOGE(TAG, "pthread_mutex_trylock(&audioEngineLock) fail");
        return -1;
    }

    result = (*recorderRecord)->SetRecordState(recorderRecord, SL_RECORDSTATE_RECORDING);
    if (SL_RESULT_SUCCESS != result) {
        LOGE(TAG, "SetRecordState fail");
        return -1;
    }

    result = (*recorderBufferQueue)->Clear(recorderBufferQueue);
    if (SL_RESULT_SUCCESS != result) {
        LOGE(TAG, "Clear fail");
        return -1;
    }

    result = (*recorderBufferQueue)->Enqueue(recorderBufferQueue, buffer->getRecordBuffer(),
                                             recorder_size);
    if (SL_RESULT_SUCCESS != result) {
        LOGE(TAG, "Enqueue fail");
        return -1;
    }
    return 0;
}

/**
 * 释放录音资源
 * @return  0：success other：fail
 */
int destroy_recorder() {

    (*recorderRecord)->SetRecordState(recorderRecord, SL_RECORDSTATE_STOPPED);
    if (recorderObject != NULL) {
        (*recorderObject)->Destroy(recorderObject);
        recorderObject = NULL;
        recorderRecord = NULL;
        recorderBufferQueue = NULL;
    }

    // destroy engine object, and invalidate all associated interfaces
    if (engineObject != NULL) {
        (*engineObject)->Destroy(engineObject);
        engineObject = NULL;
        engineEngine = NULL;
    }
    if (pcmFile != NULL){
        fclose(pcmFile);
    }
    return 0;
}


/**
 * 暂停录音
 * @return 0：sucess other: fail
 */
int pause_recorder() {
    if (NULL != recorderRecord) {
        return (*recorderRecord)->SetRecordState(recorderRecord, SL_RECORDSTATE_PAUSED);
    }
    return -1;
}

/**
 * 继续录音
 * @return 0：success other:fail
 */
int continue_recorder() {
    if (NULL != recorderRecord) {
        return (*recorderRecord)->SetRecordState(recorderRecord, SL_RECORDSTATE_RECORDING);
    }
    return -1;
}


/**
 * 设置debug文件
 * @param path     文件路径
 * @param length   路径长度
 */
void set_debug_file_path(const char *path, int length) {
    if ((NULL == path) || (length == 0)) {
        LOGE(TAG, "(NULL == path) || (length == 0)");
        return;
    }
    pcmFile = fopen(path, "w");
    if (NULL == pcmFile){
        LOGE(TAG, "file open fail: %s", path);
        return ;
    }
    LOGD(TAG, "file open success");
}









