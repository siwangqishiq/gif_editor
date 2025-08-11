package panyi.xyz.gifeditor.activity;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Context;
import android.content.Intent;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.Window;
import android.view.WindowManager;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import panyi.xyz.gifeditor.NativeBridge;

public class MainActivity extends AppCompatActivity {

    public static void start(Context context, String filepath){
        final Intent it = new Intent(context, MainActivity.class);
        it.putExtra(PARAMS_FILE_PATH, filepath);
        context.startActivity(it);
    }

    public static final String PARAMS_FILE_PATH = "FilePath";
    private MainView mMainView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);

        NativeBridge.setAndroidAssetManager(getAssets());
        mMainView = new MainView(this, getIntent().getStringExtra(PARAMS_FILE_PATH));
        setContentView(mMainView);
    }

    @Override
    protected void onDestroy() {
        if(mMainView != null){
            mMainView.postRunnableRenderThread(NativeBridge::free);
        }
        super.onDestroy();
    }

    private static class MainView extends GLSurfaceView implements GLSurfaceView.Renderer {
        private final String mFilePath;

        public MainView(Context context,String path) {
            super(context);
            mFilePath = path;
            initView(context);
        }

        @Override
        public boolean onTouchEvent(final MotionEvent event) {
            postRunnableRenderThread(()->{
                NativeBridge.handleOnAction(event.getAction() , event.getX() , event.getY());
            });
            return true;
        }

        private void initView(Context context){
            setEGLContextClientVersion(3);
            setEGLConfigChooser(8, 8, 8, 8, 16, 8);
            setRenderer(this);
            setRenderMode(RENDERMODE_CONTINUOUSLY);
        }

        @Override
        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            NativeBridge.init(mFilePath);
        }

        @Override
        public void onSurfaceChanged(GL10 gl, int width, int height) {
            NativeBridge.resize(width , height);
        }

        public void postRunnableRenderThread(Runnable runnable){
            queueEvent(runnable);
        }

        @Override
        public void onDrawFrame(GL10 gl) {
            NativeBridge.tick();
        }
    }//end inner class
}//end class