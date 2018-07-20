/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_phuket_tour_camera_preview_ChangbaRecordingPreviewScheduler */

#ifndef _Included_com_lyman_camerapreview_preview_LRecordingPreviewScheduler
#define _Included_com_lyman_camerapreview_preview_LRecordingPreviewScheduler
#ifdef __cplusplus
extern "C" {
#endif


/*
 * Class:     com_lyman_camerapreview_preview_LRecordingPreviewScheduler
 * Method:    switchCameraFacing
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_lyman_camerapreview_preview_LPreviewScheduler_switchCameraFacing
  (JNIEnv *, jobject);

/*
 * Class:     com_lyman_camerapreview_preview_LRecordingPreviewScheduler
 * Method:    prepareEGLContext
 * Signature: (Ljava/lang/Object;III)V
 */
JNIEXPORT void JNICALL Java_com_lyman_camerapreview_preview_LPreviewScheduler_prepareEGLContext
  (JNIEnv *, jobject, jobject, jint, jint, jint);

/*
 * Class:     com_lyman_camerapreview_preview_LRecordingPreviewScheduler
 * Method:    createWindowSurface
 * Signature: (Ljava/lang/Object;)V
 */
JNIEXPORT void JNICALL Java_com_lyman_camerapreview_preview_LPreviewScheduler_createWindowSurface
  (JNIEnv *, jobject, jobject);

/*
 * Class:     com_lyman_camerapreview_preview_LRecordingPreviewScheduler
 * Method:    resetRenderSize
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_com_lyman_camerapreview_preview_LPreviewScheduler_resetRenderSize
  (JNIEnv *, jobject, jint, jint);

/*
 * Class:     com_lyman_camerapreview_preview_LRecordingPreviewScheduler
 * Method:    destroyWindowSurface
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_lyman_camerapreview_preview_LPreviewScheduler_destroyWindowSurface
  (JNIEnv *, jobject);

/*
 * Class:     com_lyman_camerapreview_preview_LRecordingPreviewScheduler
 * Method:    destroyEGLContext
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_lyman_camerapreview_preview_LPreviewScheduler_destroyEGLContext
  (JNIEnv *, jobject);

/*
 * Class:     com_lyman_camerapreview_preview_LRecordingPreviewScheduler
 * Method:    notifyFrameAvailable
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_lyman_camerapreview_preview_LPreviewScheduler_notifyFrameAvailable
  (JNIEnv *, jobject);

#ifdef __cplusplus
}
#endif
#endif
