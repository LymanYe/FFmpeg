package com.lyman.camerapreview;

import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.Toast;

import com.lyman.camerapreview.preview.LPreviewScheduler;
import com.lyman.camerapreview.preview.LPreviewView;
import com.lyman.camerapreview.preview.LVideoCamera;

public class MainActivity extends AppCompatActivity {

    private RelativeLayout recordScreen;
    private LPreviewView surfaceView;
    private LVideoCamera videoCamera;
    private LPreviewScheduler previewScheduler;

    private ImageView switchCameraBtn;
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
    }

    private void findView() {
        recordScreen = findViewById(R.id.recordscreen);
        switchCameraBtn = findViewById(R.id.img_switch_camera);
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
