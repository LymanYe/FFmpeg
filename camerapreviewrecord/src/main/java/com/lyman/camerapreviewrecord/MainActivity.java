package com.lyman.camerapreviewrecord;

import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.Toast;

import com.lyman.camerapreviewrecord.preview.LPreviewScheduler;
import com.lyman.camerapreviewrecord.preview.LPreviewView;
import com.lyman.camerapreviewrecord.preview.LVideoCamera;

public class MainActivity extends AppCompatActivity {

    private RelativeLayout recordScreen;
    private LPreviewView surfaceView;
    private LVideoCamera videoCamera;
    private LPreviewScheduler previewScheduler;

    private ImageView switchCameraBtn;
    private Button recordBtn;
    private boolean isEncoding;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        findView();
        bindListener();
        initCameraResource();
    }

    private void bindListener() {
        switchCameraBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                previewScheduler.switchCameraFacing();
            }
        });
        recordBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String text = "";
                if(!isEncoding) {
                    isEncoding = true;
                    text = "停止录制";
                    int width = 360;
                    int height = 640;
                    int videoBitRate = 700 * 1024;
                    int frameRate = 24;
                    boolean useHardWareEncoding = false;
                    String outputPath = "/mnt/sdcard/preview_soft.h264";
//					boolean useHardWareEncoding = true;
//					String outputPath = "/mnt/sdcard/a_songstudio/preview_hw.h264";
                    previewScheduler.startEncoding(width, height, videoBitRate, frameRate, useHardWareEncoding, outputPath);
                } else {
                    isEncoding = false;
                    text = "开始录制";
                    previewScheduler.stopEncoding();
                    Toast.makeText(MainActivity.this,"/mnt/sdcard/preview_soft.h264查看",Toast.LENGTH_SHORT).show();
                }
                recordBtn.setText(text);
            }
        });
    }

    private void findView() {
        recordScreen = findViewById(R.id.recordscreen);
        switchCameraBtn = findViewById(R.id.img_switch_camera);
        recordBtn = findViewById(R.id.record_btn);
        surfaceView = new LPreviewView(this);
        recordScreen.addView(surfaceView, 0);
        surfaceView.getLayoutParams().width = getWindowManager().getDefaultDisplay().getWidth();
        surfaceView.getLayoutParams().height = getWindowManager().getDefaultDisplay().getWidth();
    }

    private void initCameraResource() {
        videoCamera = new LVideoCamera(this);
        previewScheduler = new LPreviewScheduler(surfaceView, videoCamera) {
            public void onPermissionDismiss(final String tip) {
                mHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        Toast.makeText(MainActivity.this, tip, Toast.LENGTH_SHORT).show();
                    }
                });
            }
        };
    }

    private Handler mHandler = new Handler() {
        public void handleMessage(Message msg) {
        }
    };
}
