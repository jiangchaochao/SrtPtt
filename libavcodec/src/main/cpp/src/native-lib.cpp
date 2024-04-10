
#include <iostream>
#include <jni.h>
#include <opus.h>
#include <srt/srt.h>
#include "audio.h"

extern "C"
JNIEXPORT
jstring JNICALL
Java_com_jiangc_libavcodec_AvCodecManager_getLibsVersion(JNIEnv *env, jobject thiz) {
    const char * opus_version = opus_get_version_string();
    uint32_t srt_version = srt_getversion();
    std::string str =  std::string("opus: ") + opus_version + std::string ("\n") + std::string ("srt: ")+  std::to_string(srt_version);
    return env->NewStringUTF(str.c_str());
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_jiangc_libavcodec_AvCodecManager_createAudioEngine(JNIEnv *env, jobject thiz) {
    return create_engine();
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_jiangc_libavcodec_AvCodecManager_createAudioRecorder(JNIEnv *env, jobject thiz) {
    return create_recorder(0, 0, 2, 0);
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_jiangc_libavcodec_AvCodecManager_startAudioRecorder(JNIEnv *env, jobject thiz) {
    return start_recording();
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_jiangc_libavcodec_AvCodecManager_destroyAudioRecorder__(JNIEnv *env, jobject thiz) {
    return destroy_recorder();
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_jiangc_libavcodec_AvCodecManager_pauseAudioRecorder(JNIEnv *env, jobject thiz) {
    return pause_recorder();
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_jiangc_libavcodec_AvCodecManager_continueAudioRecorder(JNIEnv *env, jobject thiz) {
    return continue_recorder();
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_jiangc_libavcodec_AvCodecManager_debugFile(JNIEnv *env, jobject thiz, jstring path) {
    const char *str = env->GetStringUTFChars(path, NULL);
    int length = env->GetStringLength(path);
    set_debug_file_path(str, length);
    env->ReleaseStringUTFChars(path, str);
    return 0;
}