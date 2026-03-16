#include "swapchain.h"
#include <algorithm>
#include <limits>
#include <print>
#include <stdexcept>

namespace Engine {

VkSurfaceFormatKHR Swapchain::choose_surface_format(const std::vector<VkSurfaceFormatKHR>& formats)
{
    for (const auto& fmt : formats)
        if (fmt.format == VK_FORMAT_B8G8R8A8_SRGB &&
            fmt.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            return fmt;
    return formats[0];
}

VkPresentModeKHR Swapchain::choose_present_mode(const std::vector<VkPresentModeKHR>& modes)
{
    for (const auto& mode : modes)
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
            return mode;
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Swapchain::choose_extent(const VkSurfaceCapabilitiesKHR& caps)
{
    if (caps.currentExtent.width != std::numeric_limits<u32>::max())
        return caps.currentExtent;

    int w, h;
    glfwGetFramebufferSize(m_window, &w, &h);

    VkExtent2D extent = { static_cast<u32>(w), static_cast<u32>(h) };
    extent.width  = std::clamp(extent.width,  caps.minImageExtent.width,  caps.maxImageExtent.width);
    extent.height = std::clamp(extent.height, caps.minImageExtent.height, caps.maxImageExtent.height);
    return extent;
}

void Swapchain::create(VkDevice device, VkPhysicalDevice physical_device,
                       VkSurfaceKHR surface, GLFWwindow* window,
                       Vulkan_Context& ctx)
{
    m_device          = device;
    m_physical_device = physical_device;
    m_surface         = surface;
    m_window          = window;

    Vulkan_Context::SwapChainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physical_device, m_surface, &details.capabilities);

    u32 format_count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_physical_device, m_surface, &format_count, nullptr);
    if (format_count) {
        details.formats.resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_physical_device, m_surface, &format_count, details.formats.data());
    }

    u32 mode_count = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(m_physical_device, m_surface, &mode_count, nullptr);
    if (mode_count) {
        details.presentModes.resize(mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(m_physical_device, m_surface, &mode_count, details.presentModes.data());
    }

    VkSurfaceFormatKHR surface_format = choose_surface_format(details.formats);
    VkPresentModeKHR   present_mode   = choose_present_mode(details.presentModes);
    VkExtent2D         extent         = choose_extent(details.capabilities);

    u32 image_count = details.capabilities.minImageCount + 1;
    if (details.capabilities.maxImageCount > 0 &&
        image_count > details.capabilities.maxImageCount)
        image_count = details.capabilities.maxImageCount;

    VkSwapchainCreateInfoKHR create_info{};
    create_info.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface          = m_surface;
    create_info.minImageCount    = image_count;
    create_info.imageFormat      = surface_format.format;
    create_info.imageColorSpace  = surface_format.colorSpace;
    create_info.imageExtent      = extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    auto families = ctx.get_queue_families();
    u32 queue_family_indices[] = {
        families.graphicsFamily.value(),
        families.presentFamily.value()
    };

    if (families.graphicsFamily != families.presentFamily) {
        create_info.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices   = queue_family_indices;
    } else {
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    create_info.preTransform   = details.capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode    = present_mode;
    create_info.clipped        = VK_TRUE;
    create_info.oldSwapchain   = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(m_device, &create_info, nullptr, &m_swapchain) != VK_SUCCESS)
        throw std::runtime_error("failed to create swapchain");

    vkGetSwapchainImagesKHR(m_device, m_swapchain, &image_count, nullptr);
    m_images.resize(image_count);
    vkGetSwapchainImagesKHR(m_device, m_swapchain, &image_count, m_images.data());

    m_format = surface_format.format;
    m_extent = extent;

    std::print("[swapchain] created ({}x{}, {} images)\n", extent.width, extent.height, image_count);
}

void Swapchain::create_image_views()
{
    m_image_views.resize(m_images.size());

    for (usize i = 0; i < m_images.size(); i++) {
        VkImageViewCreateInfo create_info{};
        create_info.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        create_info.image                           = m_images[i];
        create_info.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
        create_info.format                          = m_format;
        create_info.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        create_info.subresourceRange.baseMipLevel   = 0;
        create_info.subresourceRange.levelCount     = 1;
        create_info.subresourceRange.baseArrayLayer = 0;
        create_info.subresourceRange.layerCount     = 1;

        if (vkCreateImageView(m_device, &create_info, nullptr, &m_image_views[i]) != VK_SUCCESS)
            throw std::runtime_error("failed to create image view");
    }

    std::print("[swapchain] image views created ({})\n", m_image_views.size());
}

void Swapchain::create_framebuffers(VkRenderPass render_pass)
{
    m_framebuffers.resize(m_image_views.size());

    for (usize i = 0; i < m_image_views.size(); i++) {
        VkImageView attachments[] = { m_image_views[i] };

        VkFramebufferCreateInfo fb_info{};
        fb_info.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fb_info.renderPass      = render_pass;
        fb_info.attachmentCount = 1;
        fb_info.pAttachments    = attachments;
        fb_info.width           = m_extent.width;
        fb_info.height          = m_extent.height;
        fb_info.layers          = 1;

        if (vkCreateFramebuffer(m_device, &fb_info, nullptr, &m_framebuffers[i]) != VK_SUCCESS)
            throw std::runtime_error("failed to create framebuffer");
    }

    std::print("[swapchain] framebuffers created ({})\n", m_framebuffers.size());
}

void Swapchain::destroy()
{
    if (m_device == VK_NULL_HANDLE) return;

    for (auto fb : m_framebuffers)
        vkDestroyFramebuffer(m_device, fb, nullptr);
    for (auto view : m_image_views)
        vkDestroyImageView(m_device, view, nullptr);

    vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);

    m_framebuffers.clear();
    m_image_views.clear();
    m_images.clear();
    m_swapchain = VK_NULL_HANDLE;
    m_device    = VK_NULL_HANDLE;
}

} // namespace Engine
