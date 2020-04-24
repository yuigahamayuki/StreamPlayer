// ������

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

	void initPara(ReadBuffer& readBuffer);		// ��ʼ������������
private:
	AVCodecContext* _codecContext;
};