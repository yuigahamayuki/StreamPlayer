// ������

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

	void initPara(ReadBuffer& readBuffer);		// ��ʼ������������
private:
	AVCodecContext* _codecContext;
	Status* _statusPtr;
};