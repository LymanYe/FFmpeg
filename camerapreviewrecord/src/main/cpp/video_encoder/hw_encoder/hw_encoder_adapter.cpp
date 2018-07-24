#include "./hw_encoder_adapter.h"

#define LOG_TAG "HWEncoderAdapter"

HWEncoderAdapter::HWEncoderAdapter(JavaVM *g_jvm, jobject obj) {
	outputBuffer = NULL;
	queue = new MessageQueue("HWEncoder message queue");
	handler = new HWEncoderHandler(this, queue);
	this->g_jvm = g_jvm;
	this->obj = obj;
}

HWEncoderAdapter::~HWEncoderAdapter() {
}

void HWEncoderAdapter::createHWEncoder(JNIEnv *env) {
	jclass jcls = env->GetObjectClass(obj);
	jmethodID createMediaCodecSurfaceEncoderFunc = env->GetMethodID(jcls,
			"createMediaCodecSurfaceEncoderFromNative", "(IIII)V");
	env->CallVoidMethod(obj, createMediaCodecSurfaceEncoderFunc, videoWidth, videoHeight,
			videoBitRate, (int) frameRate);
	jmethodID getEncodeSurfaceFromNativeFunc = env->GetMethodID(jcls,
			"getEncodeSurfaceFromNative", "()Landroid/view/Surface;");
	jobject surface = env->CallObjectMethod(obj,
			getEncodeSurfaceFromNativeFunc);
	// 2 create window surface
	ANativeWindow* encoderWindow = ANativeWindow_fromSurface(env, surface);
	encoderSurface = eglCore->createWindowSurface(encoderWindow);
	renderer = new VideoGLSurfaceRender();
	renderer->init(videoWidth, videoHeight);
}

void HWEncoderAdapter::destroyHWEncoder(JNIEnv *env) {
	jclass jcls = env->GetObjectClass(obj);
	jmethodID closeMediaCodecFunc = env->GetMethodID(jcls, "closeMediaCodecCalledFromNative", "()V");
	env->CallVoidMethod(obj, closeMediaCodecFunc);
	// 2 release surface
	if (EGL_NO_SURFACE != encoderSurface) {
		eglCore->releaseSurface(encoderSurface);
		encoderSurface = EGL_NO_SURFACE;
	}
	if(renderer){
		LOGI("delete renderer..");
		renderer->dealloc();
		delete renderer;
		renderer = NULL;
	}
}

void HWEncoderAdapter::createEncoder(EGLCore* eglCore, int inputTexId) {
	this->eglCore = eglCore;
	this->texId = inputTexId;
	JNIEnv *env;
	int status = 0;
	bool needAttach = false;
	status = g_jvm->GetEnv((void **) (&env), JNI_VERSION_1_4);
	if (status < 0) {
		if (g_jvm->AttachCurrentThread(&env, NULL) != JNI_OK) {
			LOGE("%s: AttachCurrentThread() failed", __FUNCTION__);
			return;
		}
		needAttach = true;
	}

	// 1 create encoder
	this->createHWEncoder(env);
	// 2 allocate output memory
	LOGI("width is %d %d", videoWidth, videoHeight);
	jbyteArray tempOutputBuffer = env->NewByteArray(videoWidth*videoHeight*3/2);   // big enough
	outputBuffer = static_cast<jbyteArray>(env->NewGlobalRef(tempOutputBuffer));
	env->DeleteLocalRef(tempOutputBuffer);

	if (needAttach) {
		if (g_jvm->DetachCurrentThread() != JNI_OK) {
			LOGE("%s: DetachCurrentThread() failed", __FUNCTION__);
		}
	}
	spsppsBuffer = NULL;
	// 3 Starting Encode Thread
	pthread_create(&mEncoderThreadId, 0, encoderThreadCallback, this);
	startTime = -1;
	isEncoding = true;
	isSPSUnWriteFlag = true;
}

void* HWEncoderAdapter::encoderThreadCallback(void *myself) {
	HWEncoderAdapter *encoder = (HWEncoderAdapter*) myself;
	encoder->encodeLoop();
	pthread_exit(0);
	return 0;
}

void HWEncoderAdapter::destroyEncoder() {
	// 1 Stop Encoder Thread
	handler->postMessage(new Message(MESSAGE_QUEUE_LOOP_QUIT_FLAG));
	pthread_join(mEncoderThreadId, 0);
	if (queue) {
		queue->abort();
		delete queue;
		queue = NULL;
	}
	delete handler;
	handler = NULL;
	LOGI("before destroyEncoder");
	JNIEnv *env;
	int status = 0;
	bool needAttach = false;
	status = g_jvm->GetEnv((void **) (&env), JNI_VERSION_1_4);
	if (status < 0) {
		if (g_jvm->AttachCurrentThread(&env, NULL) != JNI_OK) {
			LOGE("%s: AttachCurrentThread() failed", __FUNCTION__);
			return;
		}
		needAttach = true;
	}
	// 2 release encoder
	this->destroyHWEncoder(env);
	// 3 release output memory
	if (outputBuffer)
		env->DeleteGlobalRef(outputBuffer);
	if (needAttach) {
		if (g_jvm->DetachCurrentThread() != JNI_OK) {
			LOGE("%s: DetachCurrentThread() failed", __FUNCTION__);
		}
	}
	if(NULL != spsppsBuffer) {
		delete[] spsppsBuffer;
	}
	fclose(h264File);
	LOGI("after destroyEncoder");
}

void HWEncoderAdapter::encode() {
	if (startTime == -1)
		startTime = getCurrentTime();
	int64_t curTime = getCurrentTime() - startTime;
	int expectedFrameCount = (int)(curTime / 1000.0f * frameRate + 0.5f);
	if (expectedFrameCount < encodedFrameCount) {
		// need drop frames
		return;
	}
	encodedFrameCount++;
	if(EGL_NO_SURFACE != encoderSurface){
		eglCore->makeCurrent(encoderSurface);
		renderer->renderToView(texId, videoWidth, videoHeight);
		eglCore->setPresentationTime(encoderSurface, ((khronos_stime_nanoseconds_t) curTime) * 1000000);
		handler->postMessage(new Message(FRAME_AVAILIBLE));
		if (!eglCore->swapBuffers(encoderSurface)) {
			LOGE("eglSwapBuffers(encoderSurface) returned error %d", eglGetError());
		}
	}
}

void HWEncoderAdapter::encodeLoop() {
	while (isEncoding) {
		Message* msg = NULL;
		if(queue->dequeueMessage(&msg, true) > 0){
		    if(MESSAGE_QUEUE_LOOP_QUIT_FLAG == msg->execute()){
		    		isEncoding = false;
		    }
		    delete msg;
		}
	}
	LOGI("HWEncoderAdapter encode Thread ending...");
}

void HWEncoderAdapter::drainEncodedData() {
	JNIEnv *env;
	int status = 0;
	bool needAttach = false;
	status = g_jvm->GetEnv((void **) (&env), JNI_VERSION_1_4);
	if (status < 0) {
		if (g_jvm->AttachCurrentThread(&env, NULL) != JNI_OK) {
			LOGE("%s: AttachCurrentThread() failed", __FUNCTION__);
			return;
		}
		needAttach = true;
	}
	jclass jcls = env->GetObjectClass(obj);
	jmethodID drainEncoderFunc = env->GetMethodID(jcls, "pullH264StreamFromDrainEncoderFromNative", "([B)J");
	long bufferSize = (long)env->CallLongMethod(obj, drainEncoderFunc, outputBuffer);
	byte* outputData = (uint8_t*)env->GetByteArrayElements(outputBuffer, 0);	// get data
	int size = (int)bufferSize;
//	LOGI("size is %d", size);

	jmethodID getLastPresentationTimeUsFunc = env->GetMethodID(jcls, "getLastPresentationTimeUsFromNative", "()J");
	long long lastPresentationTimeUs = (long long)env->CallLongMethod(obj, getLastPresentationTimeUsFunc);

	int timeMills = (int)(lastPresentationTimeUs/1000.0f);
	// push to queue
	int nalu_type = (outputData[4] & 0x1F);
	if (H264_NALU_TYPE_SEQUENCE_PARAMETER_SET == nalu_type) {
		spsppsBufferSize = size;
		spsppsBuffer = new byte[spsppsBufferSize];
		memcpy(spsppsBuffer, outputData, spsppsBufferSize);
	} else if(NULL != spsppsBuffer){
		if(H264_NALU_TYPE_IDR_PICTURE == nalu_type) {
			fwrite(spsppsBuffer, 1, spsppsBufferSize, h264File);
		}
		fwrite(outputData, 1, size, h264File);
	}
	env->ReleaseByteArrayElements(outputBuffer, (jbyte *)outputData, 0);
	if (needAttach) {
		if (g_jvm->DetachCurrentThread() != JNI_OK) {
			LOGE("%s: DetachCurrentThread() failed", __FUNCTION__);
		}
	}
}
