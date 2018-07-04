package com.lyman.videoplayer;

import android.view.Surface;

public class PngPreviewController {

	public native void init(String picPath);

	public native void setSurface(Surface surface);

	public native void resetSize(int width, int height);

	public native void stop();

}
