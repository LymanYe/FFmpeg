#ifndef RECORDING_HW_ENCODER_H
#define RECORDING_HW_ENCODER_H

#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

#include "../live_video_packet_queue.h"
#include "../video_encoder_adapter.h"
#include "message_queue/handler.h"
#include "message_queue/message_queue.h"

typedef enum {
	FRAME_AVAILIBLE
} HWEncoderMSGType;

class HWEncoderHandler;

class HWEncoderAdapter: public VideoEncoderAdapter  {
public:
	HWEncoderAdapter(JavaVM *g_jvm, jobject obj);
    virtual ~HWEncoderAdapter();

    virtual void createEncoder(EGLCore* eglCore, int inputTexId);
    virtual void destroyEncoder();
    virtual void encode();

	void drainEncodedData();
private:
	bool isEncoding;
	byte* spsppsBuffer;
	int spsppsBufferSize;
	bool isSPSUnWriteFlag;

	JavaVM *g_jvm;
	jobject obj;

	EGLCore* eglCore;

	HWEncoderHandler* handler;
	MessageQueue* queue;

    pthread_t mEncoderThreadId;
    static void* encoderThreadCallback(void *myself);
    void encodeLoop();

	EGLSurface encoderSurface;

	jbyteArray outputBuffer;
	void destroyHWEncoder(JNIEnv *env);
	void createHWEncoder(JNIEnv *env);
};

class HWEncoderHandler: public Handler {
	private:
		HWEncoderAdapter* encoderAdapter;
	public:
		HWEncoderHandler(HWEncoderAdapter* encoderAdapter, MessageQueue* queue) :
				Handler(queue) {
			this->encoderAdapter = encoderAdapter;
		}
		void handleMessage(Message* msg) {
			int what = msg->getWhat();
			switch (what) {
			case FRAME_AVAILIBLE:
				encoderAdapter->drainEncodedData();
				break;
			}
		}
	};
#endif // RECORDING_HW_ENCODER_H
