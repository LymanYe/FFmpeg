#include "video_encoder_adapter.h"

#define LOG_TAG "VideoEncoderAdapter"

VideoEncoderAdapter::VideoEncoderAdapter() {
	renderer = NULL;
}

VideoEncoderAdapter::~VideoEncoderAdapter() {
}

void VideoEncoderAdapter::init(const char* h264Path, int width, int height, int videoBitRate, float frameRate){
	h264File = fopen(h264Path, "wb+");
	this->videoWidth = width;
	this->videoHeight = height;
	this->frameRate = frameRate;
	this->videoBitRate = videoBitRate;
	this->encodedFrameCount = 0;
}
