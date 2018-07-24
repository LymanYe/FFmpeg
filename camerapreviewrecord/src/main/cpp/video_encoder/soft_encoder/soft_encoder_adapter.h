#ifndef RECORDING_SOFT_ENCODER_H
#define RECORDING_SOFT_ENCODER_H

#include <unistd.h>
#include "../video_encoder_adapter.h"
#include "./video_x264_encoder.h"
#include "./host_gpu_copier.h"
#include "yuy2_video_frame_pool.h"

#define PIXEL_BYTE_SIZE 2

class SoftEncoderAdapter: public VideoEncoderAdapter  {
public:
	SoftEncoderAdapter();
    virtual ~SoftEncoderAdapter();

	void createEncoder(EGLCore* eglCore, int inputTexId);
	void encode();
	void renderLoop();
	void startEncode();
	void destroyEncoder();

private:
	YUY2VideoFramePool* videoFramePool;
	/** 这是创建RenderThread的context, 要共享给我们这个EGLContext线程 **/
	EGLContext loadTextureContext;
	/** 操作纹理的FBO **/
    GLuint mFBO;
    GLuint outputTexId;
	EGLCore* eglCore;
	EGLSurface copyTexSurface;
	enum DownloadThreadMessage {
		MSG_NONE = 0, MSG_WINDOW_SET, MSG_RENDER_LOOP_EXIT
	};
	/** 提示PreviewThread **/
	pthread_mutex_t previewThreadLock;
	pthread_cond_t previewThreadCondition;

	pthread_mutex_t mLock;
	pthread_cond_t mCondition;
	enum DownloadThreadMessage _msg;
	pthread_t imageDownloadThread;
	static void* startDownloadThread(void* ptr);
	bool initialize();
	void loadTexture();
	void signalPreviewThread();
	void destroy();

    HostGPUCopier* hostGPUCopier;
    int pixelSize;

    VideoX264Encoder* encoder;
	pthread_t x264EncoderThread;
	static void* startEncodeThread(void* ptr);
};
#endif // RECORDING_X264_ENCODER_H
