// ������

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

	// ��ʼ��������������������ͼƬת��context
	void initPara(ReadBuffer& readBuffer);		
private:
	/*����һ֡�� ��packetQueue��ȡ��1��AVPacket�������������
		����ֵ1���ɹ������1֡�����ŵ�frameQueue��
		����ֵ-1�����봦1֡����Ҫ��packet���㣬��Ҫ��������������packet
	*/
	int decode();

	// ͼƬ��ʽת���������ڻᴴ��AVFrame
	AVFrame* convertFrame(const AVFrame* frame_raw);

	AVCodecContext* _codecContext;	
	SwsContext* _convertContext;		// ͼƬ��ʽת����context����Ҫ����scale

	VideoStatus* _videoStatusPtr;

	PacketQueue* _packetQueuePtr;
	FrameQueue* _frameQueuePtr;

};