/*
	äÖÈ¾Æ÷

*/

#pragma once

extern "C"
{
#include <SDL2/SDL.h>
};

class Status;
class VideoStatus;
class FrameQueue;

class Renderer
{
public:
	Renderer(Status* playerStatusPtr, VideoStatus* videoStatusPtr, FrameQueue* frameQueuePtr);
	~Renderer();

	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;

	void loop();

private:
	void renderOneFrame();

	SDL_Window*         _window;
	SDL_Renderer*       _sdl_renderer;
	SDL_Texture*        _sdl_texture;
	SDL_Rect            _sdl_rect;

	Status* _playerStatusPtr;
	VideoStatus* _videoStatusPtr;

	FrameQueue* _frameQueuePtr;
};