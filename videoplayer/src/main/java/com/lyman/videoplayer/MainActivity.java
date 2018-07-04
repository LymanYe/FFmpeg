package com.lyman.videoplayer;

import android.app.Activity;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.RelativeLayout;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

public class MainActivity extends AppCompatActivity {
    private static final String TAG = "MainActivity";
    static {
        System.loadLibrary("renderer");
    }

    private SurfaceView surfaceView;
    private RelativeLayout preview_parent_layout;

    private String picPath;

    private PngPreviewController pngPreviewController;
    private SurfaceHolder.Callback previewCallback = new SurfaceHolder.Callback() {

        public void surfaceCreated(SurfaceHolder holder) {
            pngPreviewController = new PngPreviewController();
            File file = new File(picPath);
            boolean exists = file.exists();
            pngPreviewController.init(picPath);
            pngPreviewController.setSurface(holder.getSurface());
        }

        public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
            pngPreviewController.resetSize(width, height);
        }

        public void surfaceDestroyed(SurfaceHolder holder) {
        }
    };

    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        copyAssetsFile2Phone(this,"1.png");
        setContentView(R.layout.activity_main);
        findView();
    }

    private void findView() {
        preview_parent_layout = (RelativeLayout) findViewById(R.id.preview_parent_layout);
        surfaceView = new SurfaceView(this);
        SurfaceHolder mSurfaceHolder = surfaceView.getHolder();
        mSurfaceHolder.addCallback(previewCallback);
        mSurfaceHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
        preview_parent_layout.addView(surfaceView, 0);
        surfaceView.getLayoutParams().width = getWindowManager().getDefaultDisplay().getWidth();
        surfaceView.getLayoutParams().height = getWindowManager().getDefaultDisplay().getWidth();
    }

    /**
     * On stop.
     */
    protected void onStop() {
        super.onStop();
        // Free the native renderer
        stopPlay();
    }

    protected void stopPlay() {
        Log.i("problem", "playerController.stop()...");
        if (null != pngPreviewController) {
            pngPreviewController.stop();
            pngPreviewController = null;
        }
    }

    /**
     * 将文件从assets目录，考贝到 /data/data/包名/files/ 目录中。assets 目录中的文件，会不经压缩打包至APK包中，使用时还应从apk包中导出来
     *
     * @param fileName 文件名,如aaa.txt
     */
    public void copyAssetsFile2Phone(Activity activity, String fileName) {
        try {
            InputStream inputStream = activity.getAssets().open(fileName);
            //getFilesDir() 获得当前APP的安装路径 /data/data/包名/files 目录
            File file = new File(activity.getFilesDir().getAbsolutePath() + File.separator + fileName);
            picPath = file.getAbsolutePath();
            if (!file.exists() || file.length() == 0) {
                FileOutputStream fos = new FileOutputStream(file);//如果文件不存在，FileOutputStream会自动创建文件
                int len = -1;
                byte[] buffer = new byte[1024];
                while ((len = inputStream.read(buffer)) != -1) {
                    fos.write(buffer, 0, len);
                }
                fos.flush();//刷新缓存区
                inputStream.close();
                fos.close();
                Log.i(TAG, "copyAssetsFile2Phone: "+"文件复制完毕");
            } else {
                Log.i(TAG, "copyAssetsFile2Phone: "+"文件已存在，无需复制");
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

    }
}
