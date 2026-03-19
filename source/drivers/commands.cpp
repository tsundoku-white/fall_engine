#include "commands.h"
#include <print>
#include <stdexcept>

namespace Engine {

void Commands::create(VkDevice device, u32 queue_family_index)
{
    m_device = device;

    VkCommandPoolCreateInfo pool_info{};
    pool_info.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pool_info.queueFamilyIndex = queue_family_index;

    if (vkCreateCommandPool(m_device, &pool_info, nullptr, &m_command_pool) != VK_SUCCESS)
        throw std::runtime_error("failed to create command pool");

    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool        = m_command_pool;
    alloc_info.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

    m_command_buffers.resize(MAX_FRAMES_IN_FLIGHT);
    if (vkAllocateCommandBuffers(m_device, &alloc_info, m_command_buffers.data()) != VK_SUCCESS)
        throw std::runtime_error("failed to allocate command buffers");

    std::print("[commands] pool + buffers created\n");
}

void Commands::reset(u32 frame)
{
    vkResetCommandBuffer(m_command_buffers[frame], 0);
}

void Commands::record(u32 frame, VkRenderPass render_pass, VkFramebuffer framebuffer,
                      VkExtent2D extent, VkPipeline pipeline, const Mesh& mesh)
{
    VkCommandBuffer cmd = m_command_buffers[frame];

    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(cmd, &begin_info) != VK_SUCCESS)
        throw std::runtime_error("failed to begin command buffer");

    VkClearValue clear_color = {{{0.0f, 0.0f, 0.0f, 1.0f}}};

    VkRenderPassBeginInfo rp_info{};
    rp_info.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rp_info.renderPass        = render_pass;
    rp_info.framebuffer       = framebuffer;
    rp_info.renderArea.offset = {0, 0};
    rp_info.renderArea.extent = extent;
    rp_info.clearValueCount   = 1;
    rp_info.pClearValues      = &clear_color;

    vkCmdBeginRenderPass(cmd, &rp_info, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

    VkViewport viewport{};
    viewport.width    = static_cast<float>(extent.width);
    viewport.height   = static_cast<float>(extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(cmd, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.extent = extent;
    vkCmdSetScissor(cmd, 0, 1, &scissor);

    VkBuffer     vb     = mesh.vertex_buffer().handle();
    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(cmd, 0, 1, &vb, &offset);
    vkCmdBindIndexBuffer(cmd, mesh.index_buffer().handle(), 0, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(cmd, mesh.index_count(), 1, 0, 0, 0);

    vkCmdEndRenderPass(cmd);

    if (vkEndCommandBuffer(cmd) != VK_SUCCESS)
        throw std::runtime_error("failed to record command buffer");
}

void Commands::destroy()
{
    if (m_device == VK_NULL_HANDLE) return;
    vkDestroyCommandPool(m_device, m_command_pool, nullptr);
    m_command_pool = VK_NULL_HANDLE;
    m_command_buffers.clear();
    m_device = VK_NULL_HANDLE;
}

} // namespace Engine
