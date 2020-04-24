// 解码器

#pragma once

extern "C"
{
#include <libavcodec/avcodec.h>
};

#include <ReadBuffer.h>

class Decoder
{
public:
	Decoder();
	~Decoder();

	void initPara(ReadBuffer& readBuffer);		// 初始化解码器参数
private:
	AVCodecContext* _codecContext;
};