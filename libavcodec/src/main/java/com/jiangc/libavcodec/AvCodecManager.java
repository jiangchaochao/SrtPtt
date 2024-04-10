package com.jiangc.libavcodec;

public class AvCodecManager {

    static {
        System.loadLibrary("avcodec");
    }

    private AvCodecManager(){

    }
    public native String getLibsVersion();

    private static class SingletonHolder {
        // 初始化外部类的实例
        private static final AvCodecManager INSTANCE = new AvCodecManager();
    }
    public static AvCodecManager getInstance(){
        return SingletonHolder.INSTANCE;
    }

    public native int debugFile(String path);
    public native int createAudioEngine();
    public native int createAudioRecorder();

    public native int startAudioRecorder();
    public native int destroyAudioRecorder();

    public native int pauseAudioRecorder();

    public native int continueAudioRecorder();
}
