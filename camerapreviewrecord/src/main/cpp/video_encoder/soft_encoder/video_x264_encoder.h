#ifndef VIDEO_ENCODER_X264_ENCODER_H
#define VIDEO_ENCODER_X264_ENCODER_H

#include "CommonTools.h"
#include "./color_conversion/color_conversion.h"
#include <vector>

#include "yuy2_video_frame_pool.h"

extern "C" {
#include "libavutil/opt.h"
#include "libavutil/mathematics.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#include "libavutil/imgutils.h"
#include "libavutil/samplefmt.h"
#include "libavutil/timestamp.h"
#include "libavcodec/avcodec.h"
#include "libavfilter/avfiltergraph.h"
#include "libavfilter/avcodec.h"
#include "libavfilter/buffersink.h"
#include "libavfilter/buffersrc.h"
#include "libavutil/avutil.h"
#include "libswscale/swscale.h"
}

using namespace std;

#define X264_INPUT_COLOR_FORMAT 					AV_PIX_FMT_YUV420P
#define ORIGIN_COLOR_FORMAT     					AV_PIX_FMT_YUYV422

class VideoX264Encoder {
private:
	AVCodecContext* pCodecCtx;
	AVCodec* pCodec;
	AVPacket pkt;
	/** 扔给x264编码器编码的Frame **/
	uint8_t* picture_buf;
	AVFrame* pFrame;
	/** 从预览线程那边扔过来的YUY2数据 **/
	uint8_t* yuy2_picture_buf;
	AVFrame* video_yuy2_frame;

    FILE* h264File;
	bool isSPSUnWriteFlag;
	const int delta = 30;

	int allocVideoStream(int width, int height, int videoBitRate, float frameRate);
	void allocAVFrame();
public:
	VideoX264Encoder();
    virtual ~VideoX264Encoder();

    int init(int width, int height, int videoBitRate, float frameRate, FILE* h264File);
    int encode(LiveVideoFrame *videoFrame);
    void pushToQueue(byte* buffer, int size, int timeMills, int64_t pts, int64_t dts);
    int destroy();
};
#endif // VIDEO_ENCODER_X264_ENCODER_H
