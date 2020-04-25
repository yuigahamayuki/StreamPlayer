#include "..\include\Decoder.h"

extern "C"
{
#include <libavutil/avutil.h>
};

Decoder::Decoder(Status* statusPtr)
	: _codecContext(nullptr), _statusPtr(statusPtr)
{
	
}

Decoder::~Decoder()
{
	avcodec_free_context(&_codecContext);
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
}
