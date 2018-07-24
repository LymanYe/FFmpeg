#ifndef LIVE_YUY2_PACKET_POOL_H
#define LIVE_YUY2_PACKET_POOL_H
#include "../live_video_packet_queue.h"

#define YUY2_PACKET_QUEUE_THRRESHOLD										45

typedef LiveVideoPacketQueue LiveVideoFrameQueue;
typedef LiveVideoPacket LiveVideoFrame;

class YUY2VideoFramePool {
protected:
	YUY2VideoFramePool(); //注意:构造方法私有
    static YUY2VideoFramePool* instance; //惟一实例
    /** 边录边合成---视频的YUV数据帧的queue **/
    LiveVideoFrameQueue* yuy2PacketQueue;
public:
    static YUY2VideoFramePool* GetInstance(); //工厂方法(用来获得实例)
    virtual ~YUY2VideoFramePool();

    /** 读取出来的YUY2的视频帧数据 **/
    void initYUY2PacketQueue();
    void abortYUY2PacketQueue();
    void destoryYUY2PacketQueue();
    int getYUY2Packet(LiveVideoFrame **videoPacket, bool block);
    bool pushYUY2PacketToQueue(LiveVideoFrame* videoPacket);
    int getYUY2PacketQueueSize();
    void clearYUY2PacketToQueue();
};

#endif //LIVE_YUY2_PACKET_POOL_H
