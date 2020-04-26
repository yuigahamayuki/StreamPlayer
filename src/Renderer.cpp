#include "..\include\Renderer.h"

extern "C"
{
#include <libavcodec/avcodec.h>
};

#include "Status.h"
#include "VideoStatus.h"
#include "FrameQueue.h"

Renderer::Renderer(Status* playerStatusPtr, VideoStatus* videoStatusPtr, FrameQueue* frameQueuePtr)
	: _window(nullptr), _sdl_renderer(nullptr), _sdl_texture(nullptr), 
	_playerStatusPtr(playerStatusPtr), _videoStatusPtr(videoStatusPtr), _frameQueuePtr(frameQueuePtr)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER))
	{
		av_log(nullptr, AV_LOG_ERROR, "Init SDL failure.\n");
		exit(1);
	}

	_window = SDL_CreateWindow("Stream Player",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		1680,						// 窗口宽度
		1000,						// 窗口高度
		SDL_WINDOW_RESIZABLE
	);

	_sdl_renderer = SDL_CreateRenderer(_window, -1, 0);

	// 渲染宽度（实际一帧画面的宽度）
	auto width_render = _videoStatusPtr->getWidth();
	// 渲染高度（实际一帧画面的高度）
	auto height_render = _videoStatusPtr->getHeight();


	_sdl_texture = SDL_CreateTexture(_sdl_renderer,
		SDL_PIXELFORMAT_IYUV,
		SDL_TEXTUREACCESS_STREAMING,
		width_render,
		height_render
		);

	_sdl_rect.x = 0;
	_sdl_rect.y = 0;
	_sdl_rect.w = width_render;
	_sdl_rect.h = height_render;
}

Renderer::~Renderer()
{
	SDL_Quit();
}

void Renderer::loop()
{
	while (1)
	{
		// 暂停状态
		if (_videoStatusPtr->getStatus() == VideoStatus::PLAYER_STATUS_PAUSE)
		{
			//SDL_Delay()
		}

		// 播放状态
		if (_videoStatusPtr->getStatus() == VideoStatus::PLAYER_STATUS_PLAY)
		{
			
			if (_frameQueuePtr->needSendWait())
			{
				if (_playerStatusPtr->getStatus() == Status::PLAYER_STATUS_LOOP)
					_playerStatusPtr->setStatus(Status::PLAYER_STATUS_WAIT);
			}

			if (_frameQueuePtr->needSendMore())
			{
				if (_playerStatusPtr->getStatus() == Status::PLAYER_STATUS_WAIT_SENT)
					_playerStatusPtr->setStatus(Status::PLAYER_STATUS_REQUESET_SEND);
			}

			renderOneFrame();

			SDL_Delay(40);
		}
	}
}

void Renderer::renderOneFrame()
{
	AVFrame* frame_render = _frameQueuePtr->take();

	SDL_UpdateTexture(_sdl_texture,
		&_sdl_rect,
		frame_render->data[0],
		frame_render->linesize[0]
		);

	SDL_RenderClear(_sdl_renderer);
	SDL_RenderCopy(_sdl_renderer, _sdl_texture, &_sdl_rect, &_sdl_rect);
	SDL_RenderPresent(_sdl_renderer);

	av_frame_free(&frame_render);
}
