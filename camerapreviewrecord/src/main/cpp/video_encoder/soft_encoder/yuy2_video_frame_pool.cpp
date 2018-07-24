#include "yuy2_video_frame_pool.h"

#define LOG_TAG "YUY2VideoFramePool"

YUY2VideoFramePool::YUY2VideoFramePool() {
	yuy2PacketQueue = NULL;
}

YUY2VideoFramePool::~YUY2VideoFramePool() {
}
//初始化静态成员
YUY2VideoFramePool* YUY2VideoFramePool::instance = new YUY2VideoFramePool();
YUY2VideoFramePool* YUY2VideoFramePool::GetInstance() {
	return instance;
}

/************************** YUY2的视频帧 packet queue *******************************************/

void YUY2VideoFramePool::initYUY2PacketQueue() {
	if (NULL == yuy2PacketQueue) {
		const char* name = "recording video yuy2 frame packet queue";
		yuy2PacketQueue = new LiveVideoFrameQueue(name);
	}
}

void YUY2VideoFramePool::abortYUY2PacketQueue() {
	if (NULL != yuy2PacketQueue) {
		yuy2PacketQueue->abort();
	}
}

void YUY2VideoFramePool::destoryYUY2PacketQueue() {
	if (NULL != yuy2PacketQueue) {
		delete yuy2PacketQueue;
		yuy2PacketQueue = NULL;
	}
}

int YUY2VideoFramePool::getYUY2Packet(LiveVideoFrame **videoFrame, bool block) {
	int result = -1;
	if (NULL != yuy2PacketQueue) {
		result = yuy2PacketQueue->get(videoFrame, block);
	}
	return result;
}

bool YUY2VideoFramePool::pushYUY2PacketToQueue(LiveVideoFrame* videoFrame) {
	bool dropFrame = false;
	if (NULL != yuy2PacketQueue) {
		yuy2PacketQueue->put(videoFrame);
	}
	return dropFrame;
}

int YUY2VideoFramePool::getYUY2PacketQueueSize() {
	if (NULL != yuy2PacketQueue) {
		return yuy2PacketQueue->size();
	}
	return 0;
}

void YUY2VideoFramePool::clearYUY2PacketToQueue() {
	if (NULL != yuy2PacketQueue) {
		return yuy2PacketQueue->flush();
	}
}
/************************** YUY2的视频帧 packet queue *******************************************/
