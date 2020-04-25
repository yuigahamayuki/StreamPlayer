// 解码器

#pragma once

extern "C"
{
#include <libavcodec/avcodec.h>
};

#include <ReadBuffer.h>

class Status;

class Decoder
{
public:
	Decoder(Status* statusPtr);
	~Decoder();

	void initPara(ReadBuffer& readBuffer);		// 初始化解码器参数
private:
	AVCodecContext* _codecContext;
	Status* _statusPtr;
};