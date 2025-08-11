package panyi.xyz.gifeditor;

import android.content.res.AssetManager;

public final class NativeBridge {
    static {
        System.loadLibrary("avcodec");
        System.loadLibrary("avfilter");
        System.loadLibrary("avformat");
        System.loadLibrary("avutil");
        System.loadLibrary("swresample");
        System.loadLibrary("swscale");
        System.loadLibrary("purple");
    }

    public static native void init(String path);

    public static native void tick();

    public static native void free();

    public static native void resize(int width , int height);

    public static native void setAndroidAssetManager(AssetManager mgr);

    public static native void handleOnAction(int action , float x , float y);

    public static native String ffmpegVersion();
}
