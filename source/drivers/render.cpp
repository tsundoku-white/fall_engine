#include "render.h"
#include <print>
#include <stdexcept>

namespace Engine {

const std::vector<Vertex> vertices = {
    // Bottom-left
    {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    // Bottom-right  
    {{ 0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    // Top-right
    {{ 0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    // Top-left
    {{-0.5f,  0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
};

// Indices for two triangles forming the square
const std::vector<uint32_t> indices = {
    0, 1, 2,  // First triangle (bottom-left, bottom-right, top-right)
    0, 2, 3   // Second triangle (bottom-left, top-right, top-left)
};
 
Render::Render(Vulkan_Context& ctx, Platform& platform)
{
    m_device         = ctx.get_device();
    m_graphics_queue = ctx.get_graphics_queue();
    m_present_queue  = ctx.get_present_queue();
 
    m_swapchain.create(m_device, ctx.get_physical_device(),
                       ctx.get_surface(), platform.get_handle(), ctx);
    m_swapchain.create_image_views();
    create_render_pass();
    m_pipeline.create(m_device, m_render_pass, m_swapchain.extent());
    m_swapchain.create_framebuffers(m_render_pass);
    m_commands.create(m_device, ctx.get_queue_families().graphicsFamily.value());
    m_mesh.create(ctx.get_physical_device(), m_device,
                  m_commands.pool(), m_graphics_queue,
                  vertices, indices);
    m_sync.create(m_device, m_swapchain.image_count());
}

void Render::create_render_pass()
{
    VkAttachmentDescription color_attachment{};
    color_attachment.format         = m_swapchain.format();
    color_attachment.samples        = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_ref{};
    color_ref.attachment = 0;
    color_ref.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments    = &color_ref;

    VkSubpassDependency dependency{};
    dependency.srcSubpass    = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass    = 0;
    dependency.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo create_info{};
    create_info.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    create_info.attachmentCount = 1;
    create_info.pAttachments    = &color_attachment;
    create_info.subpassCount    = 1;
    create_info.pSubpasses      = &subpass;
    create_info.dependencyCount = 1;
    create_info.pDependencies   = &dependency;

    if (vkCreateRenderPass(m_device, &create_info, nullptr, &m_render_pass) != VK_SUCCESS)
        throw std::runtime_error("failed to create render pass");

    std::print("[render] render pass created\n");
}

void Render::pass()
{
    u32 frame = m_sync.current_frame();

    VkFence         fence  = m_sync.in_flight(frame);
    VkCommandBuffer cmd    = m_commands.buffer(frame);

    vkWaitForFences(m_device, 1, &fence, VK_TRUE, UINT64_MAX);
    vkResetFences(m_device, 1, &fence);

    u32 image_index;
    vkAcquireNextImageKHR(m_device, m_swapchain.handle(), UINT64_MAX,
                          m_sync.image_available(frame), VK_NULL_HANDLE, &image_index);

    m_commands.reset(frame);
m_commands.record(frame, m_render_pass,
                  m_swapchain.framebuffer(image_index),
                  m_swapchain.extent(),
                  m_pipeline.handle(),
                  m_mesh);

    VkSemaphore wait_semaphores[]      = { m_sync.image_available(frame) };
    VkSemaphore signal_semaphores[] = { m_sync.render_finished(image_index) }; 
    VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    VkSubmitInfo submit_info{};
    submit_info.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.waitSemaphoreCount   = 1;
    submit_info.pWaitSemaphores      = wait_semaphores;
    submit_info.pWaitDstStageMask    = wait_stages;
    submit_info.commandBufferCount   = 1;
    submit_info.pCommandBuffers      = &cmd;
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores    = signal_semaphores;

    if (vkQueueSubmit(m_graphics_queue, 1, &submit_info, fence) != VK_SUCCESS)
        throw std::runtime_error("failed to submit draw command buffer");

    VkSwapchainKHR swapchains[] = { m_swapchain.handle() };
    VkPresentInfoKHR present_info{};
    present_info.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores    = signal_semaphores;
    present_info.swapchainCount     = 1;
    present_info.pSwapchains        = swapchains;
    present_info.pImageIndices      = &image_index;

    vkQueuePresentKHR(m_present_queue, &present_info);

    m_sync.advance_frame();
}

Render::~Render()
{
    vkDeviceWaitIdle(m_device);
    m_mesh.destroy();
    vkDestroyRenderPass(m_device, m_render_pass, nullptr);
}

} // namespace Engine
