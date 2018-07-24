#include "live_video_packet_queue.h"
#define LOG_TAG "LiveVideoPacketQueue"

LiveVideoPacketQueue::LiveVideoPacketQueue() {
    init();
}

LiveVideoPacketQueue::LiveVideoPacketQueue(const char* queueNameParam) {
    init();
    queueName = queueNameParam;
}

void LiveVideoPacketQueue::init() {
    int initLockCode = pthread_mutex_init(&mLock, NULL);
    int initConditionCode = pthread_cond_init(&mCondition, NULL);
    mNbPackets = 0;
    mFirst = NULL;
    mLast = NULL;
    mAbortRequest = false;
}

LiveVideoPacketQueue::~LiveVideoPacketQueue() {
    LOGI("%s ~PacketQueue ....", queueName);
    flush();
    pthread_mutex_destroy(&mLock);
    pthread_cond_destroy(&mCondition);
}

int LiveVideoPacketQueue::size() {
    pthread_mutex_lock(&mLock);
    int size = mNbPackets;
    pthread_mutex_unlock(&mLock);
    return size;
}

void LiveVideoPacketQueue::flush() {
    LOGI("\n %s flush .... and this time the queue size is %d \n", queueName, size());
    LiveVideoPacketList *pkt, *pkt1;
    LiveVideoPacket *videoPacket;
    pthread_mutex_lock(&mLock);
    for (pkt = mFirst; pkt != NULL; pkt = pkt1) {
        pkt1 = pkt->next;
        videoPacket = pkt->pkt;
        if (NULL != videoPacket) {
            delete videoPacket;
        }
        delete pkt;
        pkt = NULL;
    }
    mLast = NULL;
    mFirst = NULL;
    mNbPackets = 0;
    pthread_mutex_unlock(&mLock);
}

int LiveVideoPacketQueue::put(LiveVideoPacket* pkt) {
	if (mAbortRequest) {
		delete pkt;
		return -1;
	}
	LiveVideoPacketList *pkt1 = new LiveVideoPacketList();
	if (!pkt1)
		return -1;
	pkt1->pkt = pkt;
	pkt1->next = NULL;
	int getLockCode = pthread_mutex_lock(&mLock);
	if (mLast == NULL) {
		mFirst = pkt1;
	} else {
		mLast->next = pkt1;
	}
	mLast = pkt1;
	mNbPackets++;
	pthread_cond_signal(&mCondition);
	pthread_mutex_unlock(&mLock);
	return 0;
}

/* return < 0 if aborted, 0 if no packet and > 0 if packet.  */
int LiveVideoPacketQueue::get(LiveVideoPacket **pkt, bool block) {
    LiveVideoPacketList *pkt1;
    int ret;
    int getLockCode = pthread_mutex_lock(&mLock);
    for (;;) {
        if (mAbortRequest) {
            ret = -1;
            break;
        }
        pkt1 = mFirst;
        if (pkt1) {
            mFirst = pkt1->next;
            if (!mFirst)
                mLast = NULL;
            mNbPackets--;
            *pkt = pkt1->pkt;
            delete pkt1;
            pkt1 = NULL;
            ret = 1;
            break;
        } else if (!block) {
            ret = 0;
            break;
        } else {
            pthread_cond_wait(&mCondition, &mLock);
        }
    }
    pthread_mutex_unlock(&mLock);
    return ret;
}

void LiveVideoPacketQueue::abort() {
    pthread_mutex_lock(&mLock);
    mAbortRequest = true;
    pthread_cond_signal(&mCondition);
    pthread_mutex_unlock(&mLock);
}
