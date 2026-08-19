#include "SDL3/SDL_error.h"
#include "SDL3/SDL_gpu.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_iostream.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_oldnames.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_stdinc.h"
#include <cstddef>
#include <SDL3/SDL.h>
#include <span>
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>
#include <iostream>
#include <filesystem>
#include <array>
#include "dl_imgui_utils.h"
#include "AppData.h"
#include "stb_image.h"
#include "ShaderUtils.h"

SDL_Surface* LoadImage(const char* imgName, int desiredChannels)
{
    int width {0}, height {0}, channels {0};
    stbi_uc* textureAtlasBmp {stbi_load(imgName, &width, &height, &channels, 4)};
    return SDL_CreateSurfaceFrom(width, height, SDL_PIXELFORMAT_RGBA32, textureAtlasBmp, width * 4);
}

typedef struct PositionTextureVertex
{
	float x, y, z;
	float u, v;
} PositionTextureVertex;

struct Vertex 
{
    union {
        float x;
        float r;
    };
    union {
        float y;
        float g;
    };
    union {
        float z;
        float b;
    }; 
};
/*
bool CreateSamplers(AppData* appData)
{
    	// PointClamp
	appData->samplers[0] = SDL_CreateGPUSampler(appData->device, &SDL_GPUSamplerCreateInfo{
		.min_filter = SDL_GPU_FILTER_NEAREST,
		.mag_filter = SDL_GPU_FILTER_NEAREST,
		.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST,
		.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
		.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
		.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
	});
	// PointWrap
	appData->samplers[1] = SDL_CreateGPUSampler(appData->device, &(SDL_GPUSamplerCreateInfo){
		.min_filter = SDL_GPU_FILTER_NEAREST,
		.mag_filter = SDL_GPU_FILTER_NEAREST,
		.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST,
		.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
		.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
		.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
	});
	// LinearClamp
	appData->samplers[2] = SDL_CreateGPUSampler(appData->device, &(SDL_GPUSamplerCreateInfo){
		.min_filter = SDL_GPU_FILTER_LINEAR,
		.mag_filter = SDL_GPU_FILTER_LINEAR,
		.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR,
		.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
		.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
		.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
	});
	// LinearWrap
	appData->samplers[3] = SDL_CreateGPUSampler(appData->device, &(SDL_GPUSamplerCreateInfo){
		.min_filter = SDL_GPU_FILTER_LINEAR,
		.mag_filter = SDL_GPU_FILTER_LINEAR,
		.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR,
		.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
		.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
		.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
	});
	// AnisotropicClamp
	appData->samplers[4] = SDL_CreateGPUSampler(appData->device, &(SDL_GPUSamplerCreateInfo){
		.min_filter = SDL_GPU_FILTER_LINEAR,
		.mag_filter = SDL_GPU_FILTER_LINEAR,
		.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR,
		.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
		.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
		.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
		.max_anisotropy = 4,
		.enable_anisotropy = true

	});
	// AnisotropicWrap
	appData->samplers[5] = SDL_CreateGPUSampler(appData->device, &(SDL_GPUSamplerCreateInfo){
		.min_filter = SDL_GPU_FILTER_LINEAR,
		.mag_filter = SDL_GPU_FILTER_LINEAR,
		.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR,
		.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
		.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
		.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
		.max_anisotropy = 4,
		.enable_anisotropy = true
	});
}

*/

bool CreatePipeline(AppData* appData)
{
    SDL_GPUShader* vertexShader {LoadShader(appData->device, "OnlyPosition.vert")};
    if (vertexShader == nullptr)
    {
        SDL_Log("Couldn't create vertex shader!\n");
        return false;
    }

    SDL_GPUShader* texShader {LoadShader(appData->device, "TexturePos.vert")};
    if (vertexShader == nullptr)
    {
        SDL_Log("Couldn't create vertex shader!\n");
        return false;
    }

    SDL_GPUShader* solidColShader {LoadShader(appData->device, "ColorGradient.frag")};
    if (solidColShader == nullptr)
    {
        SDL_Log("Couldn't create fragment shader!\n");
        return false;
    }

    SDL_GPUShader* rainbowShader {LoadShader(appData->device, "SolidColor.frag")};
    if (solidColShader == nullptr)
    {
        SDL_Log("Couldn't create fragment shader!\n");
        return false;
    }


    std::array colorTargetDescriptions {
        SDL_GPUColorTargetDescription{
            .format = SDL_GetGPUSwapchainTextureFormat(appData->device, appData->window)
        }
    };

    std::array vertexBufferDescriptions{
        SDL_GPUVertexBufferDescription{
            .slot = 0,
            .pitch = sizeof(Vertex),
            .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
            .instance_step_rate = 0
        },
        SDL_GPUVertexBufferDescription{
            .slot = 1,
            .pitch = sizeof(Vertex),
            .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
            .instance_step_rate = 0
        }
    };

    std::array vertexAttributes {
        SDL_GPUVertexAttribute{
            .location = 0,
            .buffer_slot = 0,
            .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
            .offset = 0
        },
        SDL_GPUVertexAttribute{
            .location = 1,
            .buffer_slot = 1,
            .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
            .offset = 0
        }
    };

    SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo {
        .vertex_shader = vertexShader,
        .fragment_shader = solidColShader,
        .vertex_input_state = {
            .vertex_buffer_descriptions = vertexBufferDescriptions.data(),
            .num_vertex_buffers = vertexBufferDescriptions.size(),
            .vertex_attributes = vertexAttributes.data(),
            .num_vertex_attributes = vertexAttributes.size()
        },
        .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
        .rasterizer_state = {
            .fill_mode = SDL_GPU_FILLMODE_FILL,
            //.cull_mode = SDL_GPU_CULLMODE_BACK,
            //.front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE,
        },
        .target_info = {
            .color_target_descriptions = colorTargetDescriptions.data(),
            .num_color_targets = colorTargetDescriptions.size()
        }
    };

    SDL_GPUGraphicsPipelineCreateInfo pipelineBCreateInfo {
        .vertex_shader = vertexShader,
        .fragment_shader = rainbowShader,
        .vertex_input_state = {
            .vertex_buffer_descriptions = vertexBufferDescriptions.data(),
            .num_vertex_buffers = vertexBufferDescriptions.size(),
            .vertex_attributes = vertexAttributes.data(),
            .num_vertex_attributes = vertexAttributes.size()
        },
        .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
        .rasterizer_state = {
            .fill_mode = SDL_GPU_FILLMODE_FILL,
            //.cull_mode = SDL_GPU_CULLMODE_BACK,
            //.front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE,
        },
        .target_info = {
            .color_target_descriptions = colorTargetDescriptions.data(),
            .num_color_targets = colorTargetDescriptions.size()
        }
    };
    appData->pipeline = SDL_CreateGPUGraphicsPipeline(appData->device, &pipelineCreateInfo);
    if(appData->pipeline == nullptr)
    {
        SDL_Log("Couldn't create graphics pipelineA! %s", SDL_GetError());
        return false;
    }

    appData->pipelineB = SDL_CreateGPUGraphicsPipeline(appData->device, &pipelineBCreateInfo);
    if(appData->pipeline == nullptr)
    {
        SDL_Log("Couldn't create graphics pipelineB! %s", SDL_GetError());
        return false;
    }

    SDL_GPUGraphicsPipelineCreateInfo pipelineCCreateInfo {
        .vertex_shader = vertexShader,
        .fragment_shader = rainbowShader,
        .vertex_input_state = {
            .vertex_buffer_descriptions = vertexBufferDescriptions.data(),
            .num_vertex_buffers = vertexBufferDescriptions.size(),
            .vertex_attributes = vertexAttributes.data(),
            .num_vertex_attributes = vertexAttributes.size()
        },
        .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
        .rasterizer_state = {
            .fill_mode = SDL_GPU_FILLMODE_FILL,
            //.cull_mode = SDL_GPU_CULLMODE_BACK,
            //.front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE,
        },
        .target_info = {
            .color_target_descriptions = colorTargetDescriptions.data(),
            .num_color_targets = colorTargetDescriptions.size()
        }
    };

    SDL_ReleaseGPUShader(appData->device, vertexShader);
    SDL_ReleaseGPUShader(appData->device, solidColShader);
    SDL_ReleaseGPUShader(appData->device, rainbowShader);
    SDL_ReleaseGPUShader(appData->device, texShader);

    return true;
}


bool CreateVertexBuffer(AppData* appData, SDL_GPUBuffer** vBuff, std::span<Vertex> vertices)
{
    appData->numVertices = vertices.size();
    Uint32 verticesSize {appData->numVertices * (Uint32)sizeof(Vertex)};
    SDL_GPUBufferCreateInfo vertexBufferCreateInfo {
        .usage = SDL_GPU_BUFFERUSAGE_VERTEX,
        .size = verticesSize
    };
    *vBuff = SDL_CreateGPUBuffer(appData->device, &vertexBufferCreateInfo);

    if(vBuff == nullptr)
    {
        SDL_Log("Couldn't create vertex buffer");
        return false;
    }
    SDL_GPUTransferBufferCreateInfo transferBufferCreateInfo {
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size = verticesSize
    };
    SDL_GPUTransferBuffer* transferBuffer {SDL_CreateGPUTransferBuffer(appData->device, &transferBufferCreateInfo)};
    if (transferBuffer == nullptr)
    {
        SDL_Log("Couldn't create transfer buffer: %s", SDL_GetError());
        return false;
    }
    Vertex* transferData {static_cast<Vertex*>(SDL_MapGPUTransferBuffer(appData->device, transferBuffer, false))};
    SDL_memcpy(transferData, vertices.data(), verticesSize);
    SDL_UnmapGPUTransferBuffer(appData->device, transferBuffer);

    SDL_GPUCommandBuffer* uploadCmdBuf {SDL_AcquireGPUCommandBuffer(appData->device)};
    if (uploadCmdBuf == nullptr)
    {
        SDL_Log("Couldn't acquire GPU command buffer: %s", SDL_GetError());
        return false;
    }

    SDL_GPUCopyPass* copyPass {SDL_BeginGPUCopyPass(uploadCmdBuf)};

    SDL_GPUTransferBufferLocation bufferLocation {
        .transfer_buffer = transferBuffer,
        .offset = 0
    };

    SDL_GPUBufferRegion bufferRegion {
        .buffer = *vBuff,
        .offset = 0,
        .size = verticesSize
    };

    SDL_UploadToGPUBuffer(copyPass, &bufferLocation, &bufferRegion, false);

    SDL_EndGPUCopyPass(copyPass);
    if (!SDL_SubmitGPUCommandBuffer(uploadCmdBuf))
    {
        SDL_Log("Couldn't submit GPU command buffer: %s", SDL_GetError());
        return false;
    }
    SDL_ReleaseGPUTransferBuffer(appData->device, transferBuffer);

    return true;
}

bool UpdateVertexBuffer(AppData* appData, SDL_GPUBuffer* vBuff, std::span<Vertex> vertices)
{
    appData->numVertices = vertices.size();
    Uint32 verticesSize {appData->numVertices * (Uint32)sizeof(Vertex)};

    SDL_GPUTransferBufferCreateInfo transferBufferCreateInfo {
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size = verticesSize
    };
    SDL_GPUTransferBuffer* transferBuffer {SDL_CreateGPUTransferBuffer(appData->device, &transferBufferCreateInfo)};
    if (transferBuffer == nullptr)
    {
        SDL_Log("Couldn't create transfer buffer: %s", SDL_GetError());
        return false;
    }
    Vertex* transferData {static_cast<Vertex*>(SDL_MapGPUTransferBuffer(appData->device, transferBuffer, true))};
    SDL_memcpy(transferData, vertices.data(), verticesSize);
    SDL_UnmapGPUTransferBuffer(appData->device, transferBuffer);

    SDL_GPUCommandBuffer* uploadCmdBuf {SDL_AcquireGPUCommandBuffer(appData->device)};
    if (uploadCmdBuf == nullptr)
    {
        SDL_Log("Couldn't acquire GPU command buffer: %s", SDL_GetError());
        return false;
    }

    SDL_GPUCopyPass* copyPass {SDL_BeginGPUCopyPass(uploadCmdBuf)};

    SDL_GPUTransferBufferLocation bufferLocation {
        .transfer_buffer = transferBuffer,
        .offset = 0
    };

    SDL_GPUBufferRegion bufferRegion {
        .buffer = vBuff,
        .offset = 0,
        .size = verticesSize
    };

    SDL_UploadToGPUBuffer(copyPass, &bufferLocation, &bufferRegion, false);

    SDL_EndGPUCopyPass(copyPass);
    if (!SDL_SubmitGPUCommandBuffer(uploadCmdBuf))
    {
        SDL_Log("Couldn't submit GPU command buffer: %s", SDL_GetError());
        return false;
    }
    SDL_ReleaseGPUTransferBuffer(appData->device, transferBuffer);

    return true;
}

bool CreateIndexBuffer(AppData* appData, SDL_GPUBuffer** vBuff, std::span<uint16_t> vertices)
{
    appData->numVertices = vertices.size();
    Uint32 verticesSize {appData->numVertices * (Uint32)sizeof(Vertex)};
    SDL_GPUBufferCreateInfo vertexBufferCreateInfo {
        .usage = SDL_GPU_BUFFERUSAGE_INDEX,
        .size = verticesSize
    };
    *vBuff = SDL_CreateGPUBuffer(appData->device, &vertexBufferCreateInfo);

    if(vBuff == nullptr)
    {
        SDL_Log("Couldn't create vertex buffer");
        return false;
    }
    SDL_GPUTransferBufferCreateInfo transferBufferCreateInfo {
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size = verticesSize
    };
    SDL_GPUTransferBuffer* transferBuffer {SDL_CreateGPUTransferBuffer(appData->device, &transferBufferCreateInfo)};
    if (transferBuffer == nullptr)
    {
        SDL_Log("Couldn't create transfer buffer: %s", SDL_GetError());
        return false;
    }
    Vertex* transferData {static_cast<Vertex*>(SDL_MapGPUTransferBuffer(appData->device, transferBuffer, false))};
    SDL_memcpy(transferData, vertices.data(), verticesSize);
    SDL_UnmapGPUTransferBuffer(appData->device, transferBuffer);

    SDL_GPUCommandBuffer* uploadCmdBuf {SDL_AcquireGPUCommandBuffer(appData->device)};
    if (uploadCmdBuf == nullptr)
    {
        SDL_Log("Couldn't acquire GPU command buffer: %s", SDL_GetError());
        return false;
    }

    SDL_GPUCopyPass* copyPass {SDL_BeginGPUCopyPass(uploadCmdBuf)};

    SDL_GPUTransferBufferLocation bufferLocation {
        .transfer_buffer = transferBuffer,
        .offset = 0
    };

    SDL_GPUBufferRegion bufferRegion {
        .buffer = *vBuff,
        .offset = 0,
        .size = verticesSize
    };

    SDL_UploadToGPUBuffer(copyPass, &bufferLocation, &bufferRegion, false);

    SDL_EndGPUCopyPass(copyPass);
    if (!SDL_SubmitGPUCommandBuffer(uploadCmdBuf))
    {
        SDL_Log("Couldn't submit GPU command buffer: %s", SDL_GetError());
        return false;
    }
    SDL_ReleaseGPUTransferBuffer(appData->device, transferBuffer);

    return true;
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
    std::cout << "App initialised\n";
    if(!SDL_InitSubSystem(SDL_INIT_VIDEO)){
        SDL_Log("Failed to init video: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

	AppData* appData {new AppData()};

    SDL_SetHint("SDL_RENDER_VSYNC", "1");
    appData->window = SDL_CreateWindow("Low Level Game", appData->width, appData->height, SDL_WINDOW_RESIZABLE);
    if(!appData->window) {
        SDL_Log("Failed to initialise window: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    //SDL_SetRenderLogicalPresentation(appData->renderer, GAMEWIDTH, GAMEHEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    SDL_ShowWindow(appData->window);

    SDL_RaiseWindow(appData->window);

    // Set flags for the shader formats which this program can use
    SDL_GPUShaderFormat gpuFlags {SDL_GPU_SHADERFORMAT_SPIRV|SDL_GPU_SHADERFORMAT_DXIL|SDL_GPU_SHADERFORMAT_MSL};

    // Create a truct which interfaces with a GPU device which meets the criteria establishefd in the flags
    appData->device = SDL_CreateGPUDevice(gpuFlags, true, NULL);
    if (appData->device == nullptr){
        SDL_Log("Coudn't create GPU Device: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Links the GPU device to a specific window
    if(!SDL_ClaimWindowForGPUDevice(appData->device, appData->window)){
        SDL_Log("Couldn't claim window for GPU device: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    // Creates a GPU pipeline, and stores it in appdata
    // A pipeline is a series of specific transformations and settings to be applied in sequence to vertices submitted to a gpu
    // Each pipelien sets up the format which vertices ran through the pipeline
    // must follow as well as setting up the requisite fragment and vertex shaders
    // Note that shaders only need to exist independently until they are bound to a
    //pipeline. After this, they are freed unless needed in other pipelines
    if (!CreatePipeline(appData))
    {
        return SDL_APP_FAILURE;
    }
    std::array vertices1 {
        Vertex{-0.75f, -0.75f, 0.0f},
        Vertex{0.0f, -0.75f, 0.0f},
        Vertex{0.0f, 0.0f, 0.0f},
        Vertex{-0.75f, 0.0f, 0.0f}
    };

    // Creates a vertex buffer on the GPU, and saves a handle to it in appData.
    // Vertex buffers cannot be resized, but their contents can be changed as the
    // program cycles. 
    // Getting any data into a vertex buffer uses a transfer buffer, an
    // SDL3-gpu trick intended to allow one to set up transfers into the GPU,
    // then keep going with CPU-side calculations without worrying about
    // synchronisation or accidentally overwriting information being uploaded
    // to the gpu
    if(!CreateVertexBuffer(appData, &appData->vertexBuffer[0], vertices1))
    {
        return SDL_APP_FAILURE;
    }
    std::array verticesCol {
        Vertex{1.0f, 0.0f, 0.0f},
        Vertex{0.0f, 1.f, 0.0f},
        Vertex{0.0f, 0.0f, 1.0f},
        Vertex{0.0f, 0.0f, 0.0f}
    };
    if(!CreateVertexBuffer(appData, &appData->vertexBuffer[1], verticesCol))
    {
        return SDL_APP_FAILURE;
    }
    std::array vertices2 {
        Vertex{0.75f, 0.75f, 0.0f},
        Vertex{0.2f, 0.75f, 0.0f},
        Vertex{0.2f, 0.0f, 0.0f},
        Vertex{0.75f, 0.0f, 0.0f}
    };
    if(!CreateVertexBuffer(appData, &appData->vertexBuffer[2], vertices2))
    {
        return SDL_APP_FAILURE;
    }
    std::array<uint16_t, 6> verticesi {
        0,1,3,
        1,2,3
    };

    // This is an index buffer, used to hold indices into existing vertex buffers
    // When rendering, this allows non-triangle shapes to be generated with far
    //greater ease, as instead of needing to repeatedly upload the same vertex, 
    // one can instead upload a vertex once, then repeatedly reuse it
    if(!CreateIndexBuffer(appData, &appData->indexBuffer, verticesi))
    {
        return SDL_APP_FAILURE;
    }
    // texture Samplers are objects used to determine how a 2d or 3d object ought 
    // map onto geometry - the algorithm to determinehow the pixels should blend, 
    // how it should change as one gets closer to or further away from it, whether 
    // it ought wrap, etc.
    //CreateSamplers(appData);
    //SDL_Surface* textSurf {LoadImage("./resources/ravioli.bmp", 4)};

    // A GPU texture is a handle to image data stored on the GPU itself. It can be
    //  accessed and rendered incredibly quickly, at the cost of being more tricky 
    // to modify. Note that GPU textures can be used for a wide array of different 
    // things, including uploadig and processing arbitrary information on the GPU


	*appstate = appData;

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event){
	AppData* appData {(AppData*)appstate};
    switch(event->type) {
        case (SDL_EVENT_QUIT) : {
            return SDL_APP_SUCCESS;
        } break;
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate){
	AppData* appData {(AppData*)appstate};
    // Get DeltaTime
    Uint64 currentTime {SDL_GetPerformanceCounter()};
    float deltaTime {(currentTime - appData->lastTime) / (float)SDL_GetPerformanceFrequency()};
    appData->lastTime = currentTime;

	// Update button timers here if any
	// Update

    static std::array verticesCol {
        Vertex{1.0f, 0.0f, 0.0f},
        Vertex{0.0f, 1.f, 0.0f},
        Vertex{0.0f, 0.0f, 1.0f},
        Vertex{0.0f, 0.0f, 0.0f}
    };
    static bool xReverse {false};
    static bool yReverse{false};
    static bool zReverse{false};

    verticesCol[0].x += xReverse ? 0.05 : -0.05;
    verticesCol[0].y  += yReverse ? 0.05 : -0.05;
    verticesCol[0].z  += zReverse ? 0.05 : -0.05;
    if(verticesCol[0].x  < 0.0f)
    {
        verticesCol[0].x = 0.0f;
        xReverse = true;
    }
    else if (verticesCol[0].x  > 1.0f)
    {
        verticesCol[0].x  = 1.0f;
        xReverse = false;
    }

    if(verticesCol[0].y < 0.0f)
    {
        verticesCol[0].y = 0.0f;
        yReverse = true;
    }
    else if (verticesCol[0].y > 1.0f)
    {
        verticesCol[0].y = 1.0f;
        yReverse = false;
    }

    if(verticesCol[0].z < 0.0f)
    {
        verticesCol[0].z = 0.0f;
        zReverse = true;
    }
    else if (verticesCol[0].z > 1.0f)
    {
        verticesCol[0].z = 1.0f;
        zReverse = false;
    }
    
    UpdateVertexBuffer(appData, appData->vertexBuffer[1], verticesCol);
    
    SDL_GPUCommandBuffer* commandBuffer {SDL_AcquireGPUCommandBuffer(appData->device)};
    if (commandBuffer == nullptr) {
        SDL_Log("Could not acquire command buffer from gpu: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_GPUTexture* swapChainTexture;
    if (!SDL_WaitAndAcquireGPUSwapchainTexture(commandBuffer, appData->window, &swapChainTexture, NULL, NULL)){
        SDL_Log("Couldn't acquire swapchain texture: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_GPUColorTargetInfo targetInfo {
        .texture = swapChainTexture,
        .clear_color = SDL_FColor{0.0f, 0.0f, 0.0f, 1.0f},
        .load_op = SDL_GPU_LOADOP_CLEAR,
        .store_op = SDL_GPU_STOREOP_STORE
    };

    SDL_GPURenderPass* renderPass {SDL_BeginGPURenderPass(commandBuffer, &targetInfo, 1, nullptr)};
    SDL_BindGPUGraphicsPipeline(renderPass, appData->pipeline);
    std::array vertexBuffers{
        SDL_GPUBufferBinding{
            .buffer = appData->vertexBuffer[0],
            .offset = 0
        },
        SDL_GPUBufferBinding{
            .buffer = appData->vertexBuffer[1],
            .offset = 0
        }
    };
    SDL_BindGPUVertexBuffers(renderPass, 0, vertexBuffers.data(), vertexBuffers.size());
    SDL_GPUBufferBinding indexBuffer{
        .buffer = appData->indexBuffer,
        .offset = 0
    };
    SDL_BindGPUIndexBuffer(renderPass, &indexBuffer, SDL_GPU_INDEXELEMENTSIZE_16BIT);
    SDL_DrawGPUIndexedPrimitives(renderPass, 6, 1, 0, 0, 0);
    SDL_BindGPUGraphicsPipeline(renderPass, appData->pipelineB);
    std::array vertexBuffers2 {
        SDL_GPUBufferBinding{
            .buffer = appData->vertexBuffer[2]
        }
    };

    SDL_BindGPUVertexBuffers(renderPass, 0, vertexBuffers2.data(), vertexBuffers2.size());
    SDL_BindGPUIndexBuffer(renderPass, &indexBuffer, SDL_GPU_INDEXELEMENTSIZE_16BIT);
    SDL_DrawGPUIndexedPrimitives(renderPass, 6, 1, 0, 0, 0);
    SDL_EndGPURenderPass(renderPass);
    SDL_SubmitGPUCommandBuffer(commandBuffer);


    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result){
	AppData* appData {(AppData*)appstate};
    SDL_ReleaseWindowFromGPUDevice(appData->device, appData->window);
    SDL_DestroyWindow(appData->window);
    SDL_DestroyGPUDevice(appData->device);

    //QuitImgui();
}