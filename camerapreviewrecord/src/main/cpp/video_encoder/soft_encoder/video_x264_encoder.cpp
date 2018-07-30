#include "video_x264_encoder.h"

#define LOG_TAG "VideoX264Encoder"

VideoX264Encoder::VideoX264Encoder() {
}

VideoX264Encoder::~VideoX264Encoder() {
}

int VideoX264Encoder::init(int width, int height, int videoBitRate, float frameRate, FILE* h264File) {
	//1:注册所有格式与编解码器
	avcodec_register_all();
	//3:为我们的Container添加Stream(包括编码器)
	if (this->allocVideoStream(width, height, videoBitRate, frameRate) < 0) {
		LOGI("alloc Video Stream Failed... \n");
		return -1;
	}
	//5:分配AVFrame存储编码之前的YUV420P的原始数据
	this->allocAVFrame();
	this->h264File = h264File;
	isSPSUnWriteFlag = true;
    return 0;
}

int VideoX264Encoder::encode(LiveVideoFrame * videoFrame) {
	memcpy(yuy2_picture_buf, videoFrame->buffer, videoFrame->size);
	yuy2_to_yuv420p((const uint8_t *const *) video_yuy2_frame->data, video_yuy2_frame->linesize, pCodecCtx->width, pCodecCtx->height, pFrame->data, pFrame->linesize);
	int presentationTimeMills = videoFrame->timeMills;
	AVRational time_base = {1, 1000};
	int64_t pts = presentationTimeMills / 1000.0f / av_q2d(time_base);
	pFrame->pts = pts;
	AVPacket pkt = { 0 };
	int got_packet;
	av_init_packet(&pkt);
	int ret = avcodec_encode_video2(pCodecCtx, &pkt, pFrame, &got_packet);
	if (ret < 0) {
		LOGI("Error encoding video frame: %s\n", av_err2str(ret));
		ret = -1;
	} else if (got_packet && pkt.size) {
		fwrite(pkt.data, 1, pkt.size, h264File);
	} else {
		LOGI("No Output Frame...");
	}
	av_free_packet(&pkt);
	return ret;
}

int VideoX264Encoder::destroy() {
	//Clean
	avcodec_close(pCodecCtx);
	av_free(pFrame);
	av_free(picture_buf);
    return 0;
}

int VideoX264Encoder::allocVideoStream(int width, int height, int videoBitRate, float frameRate) {
	pCodec = avcodec_find_encoder(AV_CODEC_ID_H264);
	if (!pCodec) {
		LOGI("Can not find encoder! \n");
		return -1;
	}
	pCodecCtx = avcodec_alloc_context3(pCodec);
	/* resolution must be a multiple of two */
	pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
	pCodecCtx->width = width;
	pCodecCtx->height = height;
	/* frames per second */
	pCodecCtx->time_base.num = 1;
	pCodecCtx->time_base.den = frameRate;
	/* emit one intra frame every ten frames */
	pCodecCtx->gop_size = (int) frameRate;
	pCodecCtx->max_b_frames = 0;

	LOGI("************* gop size is %.2f videoBitRate is %d *************", frameRate, videoBitRate);
	pCodecCtx->flags |= CODEC_FLAG_QSCALE;
	pCodecCtx->i_quant_factor = 0.8;
	pCodecCtx->qmin = 10;
	pCodecCtx->qmax = 30;
	pCodecCtx->bit_rate = videoBitRate;
	pCodecCtx->rc_min_rate = videoBitRate - delta * 1000;
	pCodecCtx->rc_max_rate = videoBitRate + delta * 1000;
	pCodecCtx->rc_buffer_size = videoBitRate * 2;
	//H.264
	// 新增语句，设置为编码延迟
	av_opt_set(pCodecCtx->priv_data, "preset", "ultrafast", 0);
	// 实时编码关键看这句，上面那条无所谓
	av_opt_set(pCodecCtx->priv_data, "tune", "zerolatency", 0);
	av_opt_set(pCodecCtx->priv_data, "profile", "main", 0);
    int result = avcodec_open2(pCodecCtx,pCodec, NULL);
	if (result < 0) {
		LOGI("Failed to open encoder! \n");
		return -1;
	}

	return 0;
}

void VideoX264Encoder::allocAVFrame() {
	// target yuv420p buffer
	pFrame = av_frame_alloc();
	int pictureInYUV420PSize = avpicture_get_size(pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);
	picture_buf = (uint8_t *) av_malloc(pictureInYUV420PSize);
	avpicture_fill((AVPicture *) pFrame, picture_buf, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);
	// origin yuy2 buffer
	video_yuy2_frame = av_frame_alloc();
	int pictureInYUY2Size = avpicture_get_size(ORIGIN_COLOR_FORMAT, pCodecCtx->width, pCodecCtx->height);
	yuy2_picture_buf = (uint8_t *) av_malloc(pictureInYUY2Size);
	avpicture_fill((AVPicture *) video_yuy2_frame, yuy2_picture_buf, ORIGIN_COLOR_FORMAT, pCodecCtx->width, pCodecCtx->height);
}
