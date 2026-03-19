#pragma once
#include "../common.h"
#include "../vulkan_context.h"
#include "../platform.h"
#include "swapchain.h"
#include "pipeline.h"
#include "buffer.h"
#include "commands.h"
#include "sync.h"

namespace Engine {

class Render {
public:
    Render(Vulkan_Context& ctx, Platform& platform);
    void pass();
    ~Render();

private:
    Platform*     m_platform       = nullptr;
    VkDevice     m_device         = VK_NULL_HANDLE;
    VkQueue      m_graphics_queue = VK_NULL_HANDLE;
    VkQueue      m_present_queue  = VK_NULL_HANDLE;
    VkRenderPass m_render_pass    = VK_NULL_HANDLE;

    Swapchain m_swapchain;
    Pipeline  m_pipeline;
    Commands  m_commands;
    Mesh      m_mesh;
    Sync      m_sync;

    void create_render_pass();
};

} // namespace Engine
