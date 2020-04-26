#include "..\include\Decoder.h"

extern "C"
{
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
};

#include "VideoStatus.h"
#include "PacketQueue.h"
#include "FrameQueue.h"

Decoder::Decoder(VideoStatus* videoStatusPtr, PacketQueue* packetQueuePtr, FrameQueue* frameQueuePtr)
	: _codecContext(nullptr), _convertContext(nullptr),
	_videoStatusPtr(videoStatusPtr),
	_packetQueuePtr(packetQueuePtr),
	_frameQueuePtr(frameQueuePtr)
{
	
}

Decoder::~Decoder()
{
	avcodec_free_context(&_codecContext);
}

void Decoder::loop()
{
	while (1)
	{
		decode();
	}
}

void Decoder::initPara(ReadBuffer & readBuffer)
{

	AVCodecParameters* codecPara = avcodec_parameters_alloc();
	codecPara->codec_type = (AVMediaType)readBuffer.readInt8();
	codecPara->codec_id = (AVCodecID)readBuffer.readUInt32();
	codecPara->codec_tag = readBuffer.readUInt32();
	codecPara->extradata_size = readBuffer.readUInt32();
	codecPara->extradata = static_cast<uint8_t*>(av_malloc(codecPara->extradata_size + AV_INPUT_BUFFER_PADDING_SIZE));
	readBuffer.readChunk(codecPara->extradata, codecPara->extradata_size);
	codecPara->format = readBuffer.readInt16();
	codecPara->bit_rate = readBuffer.readUInt64();
	codecPara->bits_per_coded_sample = readBuffer.readUInt8();
	codecPara->bits_per_raw_sample = readBuffer.readUInt8();
	codecPara->profile = readBuffer.readUInt16();
	codecPara->level = readBuffer.readUInt16();
	codecPara->width = readBuffer.readUInt16();
	codecPara->height = readBuffer.readUInt16();
	codecPara->chroma_location = (AVChromaLocation)readBuffer.readUInt8();
	codecPara->video_delay = readBuffer.readUInt8();

	AVCodec* codec = avcodec_find_decoder(codecPara->codec_id);
	if (codec == nullptr)
	{
		av_log(nullptr, AV_LOG_ERROR, "Cannot find the decoder.\n");
		exit(1);
	}

	_codecContext = avcodec_alloc_context3(codec);
	int ret = avcodec_parameters_to_context(_codecContext, codecPara);
	if (ret < 0)
	{
		char buf[64];
		av_strerror(ret, buf, sizeof(buf));
		av_log(nullptr, AV_LOG_ERROR, "%s\n", buf);
	}

	ret = avcodec_open2(_codecContext, codec, nullptr);
	if (ret < 0)
	{
		char buf[64];
		av_strerror(ret, buf, sizeof(buf));
		av_log(nullptr, AV_LOG_ERROR, "%s\n", buf);
	}

	avcodec_parameters_free(&codecPara);


	// 初始化图片转换器
	_convertContext = sws_getContext(
		_codecContext->width,
		_codecContext->height,
		_codecContext->pix_fmt,
		_videoStatusPtr->getWidth(),
		_videoStatusPtr->getHeight(),
		AV_PIX_FMT_YUV420P,
		SWS_BICUBIC,
		nullptr,
		nullptr,
		nullptr);
}

int Decoder::decode()
{

	AVPacket* packet = _packetQueuePtr->take();

	int ret = avcodec_send_packet(_codecContext, packet);
	if (ret != 0)
	{
		if (ret == AVERROR(EAGAIN))		// 应该call receive_frame，此packet要重新送入解码器
		{
			int aa;
		}
		else if (ret == AVERROR_EOF)	//	解码器被冲刷
		{
			return 0;
		}
		else
		{
			char buf[64] = {0};
			av_strerror(ret, buf, sizeof(buf));
			av_log(nullptr, AV_LOG_ERROR, "%s\n", buf);
			exit(1);
		}
	}

	//av_packet_unref(packet);
	av_packet_free(&packet);

	AVFrame* frame_raw = av_frame_alloc();
	ret = avcodec_receive_frame(_codecContext, frame_raw);
	if (ret != 0)
	{
		av_frame_free(&frame_raw);
		
		if (ret == AVERROR(EAGAIN))		// 需要再送若干packet给解码器，即call send_packet，需要free掉先前分配的内存
		{
			
			return -1;
		}
		else if (ret == AVERROR_EOF)
		{
			return 0;
		}
		else
		{
			char buf[64] = { 0 };
			av_strerror(ret, buf, sizeof(buf));
			av_log(nullptr, AV_LOG_ERROR, "%s\n", buf);

			return -2;
		}
	}

	// 处理frame
	AVFrame* frame_render = convertFrame(frame_raw);
	av_frame_free(&frame_raw);
	//av_frame_free(&frame_render);

	_frameQueuePtr->put(frame_render);
	return 1;
}

AVFrame * Decoder::convertFrame(const AVFrame * frame_raw)
{
	AVFrame* frame_render = av_frame_alloc();

	auto buf_size = av_image_get_buffer_size(AV_PIX_FMT_YUV420P,
		_videoStatusPtr->getWidth(),
		_videoStatusPtr->getHeight(),
		1);

	uint8_t* buffer = (uint8_t *)av_malloc(buf_size);
	av_image_fill_arrays(frame_render->data,
		frame_render->linesize,
		buffer,
		AV_PIX_FMT_YUV420P, 
		_videoStatusPtr->getWidth(),
		_videoStatusPtr->getHeight(),
		1);

	sws_scale(_convertContext,
		(const uint8_t* const*)frame_raw->data,
		frame_raw->linesize,
		0,
		_codecContext->height,
		frame_render->data,
		frame_render->linesize);

	return frame_render;
}
