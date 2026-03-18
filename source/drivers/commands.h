#pragma once
#include "../common.h"
#include <vector>

namespace Engine {

class Commands {
public:
    void create(VkDevice device, u32 queue_family_index);
    void destroy();
    ~Commands() { destroy(); }

    void record(u32 frame, VkRenderPass render_pass, VkFramebuffer framebuffer,
                VkExtent2D extent, VkPipeline pipeline);
    void reset(u32 frame);

    VkCommandBuffer buffer(u32 frame) const { return m_command_buffers[frame]; }

    static constexpr i32 MAX_FRAMES_IN_FLIGHT = 2;

private:
    VkDevice      m_device       = VK_NULL_HANDLE;
    VkCommandPool m_command_pool = VK_NULL_HANDLE;

    std::vector<VkCommandBuffer> m_command_buffers;
};

} // namespace Engine
