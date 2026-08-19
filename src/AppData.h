#ifndef DL_SDL3_APPDATA_H
#define DL_SDL3_APPDATA_H
#include "SDL3/SDL_gpu.h"
#include "SDL3/SDL_stdinc.h"
#include <SDL3/SDL.h>
#include <array>

static const char* SamplerNames[] =
{
	"PointClamp",
	"PointWrap",
	"LinearClamp",
	"LinearWrap",
	"AnisotropicClamp",
	"AnisotropicWrap",
};

struct AppData {
	static constexpr int width {1280};
	static constexpr int height {720};

	SDL_GPUDevice* device {nullptr};
	SDL_Window *window {nullptr};
	SDL_GPUGraphicsPipeline* pipeline {nullptr};
	SDL_GPUGraphicsPipeline* pipelineB {nullptr};
	SDL_GPUGraphicsPipeline* pipelineC {nullptr};
	std::array<SDL_GPUBuffer*,3> vertexBuffer{nullptr};
	SDL_GPUBuffer* indexBuffer{nullptr};
	SDL_GPUTexture* texture{nullptr};
	std::array<SDL_GPUSampler*, SDL_arraysize(SamplerNames)> samplers;

	int lastTime {0};
	float deltaTime {0.0f};
	Uint32 numVertices {};

};

#endif