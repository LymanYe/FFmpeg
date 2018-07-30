package com.lyman.camerapreviewrecord.encoder;

import android.annotation.TargetApi;
import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.os.Build;
import android.util.Log;
import android.view.Surface;

import java.nio.ByteBuffer;

@TargetApi(Build.VERSION_CODES.JELLY_BEAN_MR2)
public class MediaCodecSurfaceEncoder {
	private static final String TAG = "MediaCodecSurfaceEncoder";
	private static final boolean VERBOSE = false;

	public static final String MIME_TYPE = "video/avc"; // H.264 Advanced Video
														// Coding
	private static final int IFRAME_INTERVAL = 1; // sync frame every second

	private Surface mInputSurface;
	private MediaCodec mEncoder;
	private MediaCodec.BufferInfo mBufferInfo;
	private long lastPresentationTimeUs = -1;

	public MediaCodecSurfaceEncoder(int width, int height, int bitRate, int frameRate) throws Exception {
		MediaFormat format = MediaFormat.createVideoFormat(MIME_TYPE, width, height);
		format.setInteger(MediaFormat.KEY_COLOR_FORMAT, MediaCodecInfo.CodecCapabilities.COLOR_FormatSurface);
		format.setInteger(MediaFormat.KEY_BIT_RATE, bitRate);
		format.setInteger(MediaFormat.KEY_FRAME_RATE, frameRate);
		format.setInteger(MediaFormat.KEY_I_FRAME_INTERVAL, IFRAME_INTERVAL);
		if (VERBOSE)
			Log.d(TAG, "format: " + format);
		try {
			mEncoder = MediaCodec.createEncoderByType(MIME_TYPE);
			mEncoder.configure(format, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);
			mInputSurface = mEncoder.createInputSurface();
			mEncoder.start();
		} catch (Exception e) {
			throw e;
		}
	}

	/**
	 * Returns the encoder's input surface.
	 */
	public Surface getInputSurface() {
		return mInputSurface;
	}

	/**
	 * Shuts down the encoder thread, and releases encoder resources.
	 * <p>
	 * Does not return until the encoder thread has stopped.
	 */
	public void shutdown() {
		if (VERBOSE)
			Log.d(TAG, "releasing encoder objects");
		try {
			if (null != mEncoder) {
				mEncoder.stop();
				mEncoder.release();
				mEncoder = null;
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	private static final int TIMEOUT_USEC = 5000;

	public long pullH264StreamFromDrainEncoderFromNative(byte[] returnedData) {
		long val = 0;

		try {
			mBufferInfo = new MediaCodec.BufferInfo();
			ByteBuffer[] encoderOutputBuffers = mEncoder.getOutputBuffers();
			int encoderStatus = mEncoder.dequeueOutputBuffer(mBufferInfo, TIMEOUT_USEC);
			if (encoderStatus == MediaCodec.INFO_TRY_AGAIN_LATER) {
				// no output available yet
				Log.i(TAG, "no output available yet");
			} else if (encoderStatus == MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED) {
				Log.i(TAG, " not expected for an encoder");
				encoderOutputBuffers = mEncoder.getOutputBuffers();
			} else if (encoderStatus == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
				MediaFormat mEncodedFormat = mEncoder.getOutputFormat();
				Log.d(TAG, "encoder output format changed: " + mEncodedFormat);
			} else if (encoderStatus < 0) {
				// let's ignore it
				Log.w(TAG, "unexpected result from encoder.dequeueOutputBuffer: " + encoderStatus);
			} else {
				ByteBuffer encodedData = encoderOutputBuffers[encoderStatus];
				if (encodedData == null) {
					throw new RuntimeException("encoderOutputBuffer " + encoderStatus + " was null");
				}
				if ((mBufferInfo.flags & MediaCodec.BUFFER_FLAG_CODEC_CONFIG) != 0) {
					// The codec config data was pulled out when we got the
					// INFO_OUTPUT_FORMAT_CHANGED status. The MediaMuxer won't
					// accept
					// a single big blob -- it wants separate csd-0/csd-1 chunks
					// --
					// so simply saving this off won't work.
					if (VERBOSE)
						Log.d(TAG, "ignoring BUFFER_FLAG_CODEC_CONFIG, mBufferInfo.size = " + mBufferInfo.size);

					if (mBufferInfo.size != 0) {
						encodedData.position(mBufferInfo.offset);
						encodedData.limit(mBufferInfo.offset + mBufferInfo.size);

						val = mBufferInfo.size;

						encodedData.get(returnedData, 0, mBufferInfo.size);
					} else {
						Log.i(TAG, "why mBufferInfo.size is equals 0");
					}

					mBufferInfo.size = 0;
				}
				if (mBufferInfo.presentationTimeUs >= lastPresentationTimeUs) {
					if (mBufferInfo.size != 0) {
						encodedData.position(mBufferInfo.offset);
						encodedData.limit(mBufferInfo.offset + mBufferInfo.size);
						lastPresentationTimeUs = mBufferInfo.presentationTimeUs;
						val = mBufferInfo.size;

						encodedData.get(returnedData, 0, mBufferInfo.size);

						if (VERBOSE) {
							Log.d(TAG, "sent " + mBufferInfo.size + " bytes to muxer, ts="
									+ mBufferInfo.presentationTimeUs);
						}
					} else {
						Log.i(TAG, "why mBufferInfo.size is equals 0");
					}
				} else {
					Log.d(TAG, "mBufferInfo.presentationTimeUs < lastPresentationTimeUs");
				}

				mEncoder.releaseOutputBuffer(encoderStatus, false);

				if ((mBufferInfo.flags & MediaCodec.BUFFER_FLAG_END_OF_STREAM) != 0) {
					Log.w(TAG, "reached end of stream unexpectedly");
				}
			}
		} catch (Exception e) {
			e.printStackTrace();
		}

		return val;
	}

	public long getLastPresentationTimeUs() {
		return lastPresentationTimeUs;
	}

	// temporary not supported devices list
	public static boolean IsInNotSupportedList() {
		String model = android.os.Build.MODEL;
		if (model.compareTo("Coolpad 8720L") == 0)
			return true;
		if (model.compareTo("vivo X5L") == 0)
			return true;
		if (model.compareTo("CHE-TL00H") == 0)
			return true;
		if (model.indexOf("GT-I9158V") != -1)
			return true;
		if (model.indexOf("HTC D826w") != -1)
			return true;
		if (model.indexOf("y923") != -1)
			return true;
		if (model.indexOf("R7007") != -1)
			return true;
		if (model.indexOf("P6-C00") != -1)
			return true;
		if (model.indexOf("F240L") != -1) // LG-F240L
			return true;
		if (model.indexOf("A7600") != -1) // 联想A7600
			return true;

		return false;
	}
}
