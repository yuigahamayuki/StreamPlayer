// 解码器

#pragma once

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
};

#include <ReadBuffer.h>

class VideoStatus;
class PacketQueue;
class FrameQueue;

class Decoder
{
public:
	Decoder(VideoStatus* videoStatusPtr, PacketQueue* packetQueuePtr, FrameQueue* frameQueuePtr);
	~Decoder();

	void loop();

	// 初始化解码器参数，并设置图片转换context
	void initPara(ReadBuffer& readBuffer);		
private:
	/*解码一帧， 从packetQueue上取出1个AVPacket，送入解码器，
		返回值1，成功解码出1帧，并放到frameQueue上
		返回值-1，解码处1帧所需要的packet不足，需要再往解码器放入packet
	*/
	int decode();

	// 图片格式转换
	void convertFrame();

	AVCodecContext* _codecContext;	
	SwsContext* _convertContext;		// 图片格式转换的context，主要用于scale

	VideoStatus* _videoStatusPtr;

	PacketQueue* _packetQueuePtr;
	FrameQueue* _frameQueuePtr;

	// AVFrame不能多次调用av_frame_alloc，会造成严重的内存泄漏（av_frame_free似乎并不释放data空间)
	AVFrame* _frameRaw;				// 解码器输出的frame
	AVFrame* _frameRender;			// 上面的rawFrame经过图片格式、大小转换后的frame，是实际渲染器渲染的图片
	uint8_t* _pictureBuffer;		// _frameRender数据使用的buffer

};