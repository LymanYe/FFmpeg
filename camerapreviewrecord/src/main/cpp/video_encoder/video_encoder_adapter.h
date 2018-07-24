#ifndef VIDEO_ENCODER_ADAPTER_H
#define VIDEO_ENCODER_ADAPTER_H

#include "CommonTools.h"
#include "opengl_media/render/video_gl_surface_render.h"
#include "egl_core/egl_core.h"

class VideoEncoderAdapter {
public:
	VideoEncoderAdapter();
    virtual ~VideoEncoderAdapter();

    virtual void init(const char* h264Path, int width, int height, int videoBitRate, float frameRate);
    virtual void createEncoder(EGLCore* eglCore, int inputTexId) = 0;
    virtual void encode() = 0;
    virtual void destroyEncoder() = 0;

protected:
	int encodedFrameCount;
	int videoWidth;
	int videoHeight;
	int videoBitRate;
	float frameRate;
	FILE* h264File;
	int64_t startTime;

	VideoGLSurfaceRender* renderer;
	int texId;

};
#endif // VIDEO_ENCODER_ADAPTER_H
