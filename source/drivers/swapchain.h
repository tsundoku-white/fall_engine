#pragma once
#include "../common.h"
#include "../vulkan_context.h"
#include <GLFW/glfw3.h>
#include <vector>

namespace Engine {

class Swapchain {
public:
    void create(VkDevice device, VkPhysicalDevice physical_device,
                VkSurfaceKHR surface, GLFWwindow* window,
                Vulkan_Context& ctx);
    void destroy();
    ~Swapchain() { destroy(); }

    void create_image_views();
    void create_framebuffers(VkRenderPass render_pass);

    VkSwapchainKHR                  handle()      const { return m_swapchain; }
    VkFormat                        format()      const { return m_format; }
    VkExtent2D                      extent()      const { return m_extent; }
    u32                             image_count() const { return (u32)m_images.size(); }
    VkFramebuffer                   framebuffer(u32 i) const { return m_framebuffers[i]; }
    const std::vector<VkImageView>& image_views() const { return m_image_views; }

private:
    VkDevice         m_device          = VK_NULL_HANDLE;
    VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
    VkSurfaceKHR     m_surface         = VK_NULL_HANDLE;
    GLFWwindow*      m_window          = nullptr;

    VkSwapchainKHR             m_swapchain = VK_NULL_HANDLE;
    std::vector<VkImage>       m_images;
    std::vector<VkImageView>   m_image_views;
    std::vector<VkFramebuffer> m_framebuffers;
    VkFormat                   m_format = VK_FORMAT_UNDEFINED;
    VkExtent2D                 m_extent = {};

    VkSurfaceFormatKHR choose_surface_format(const std::vector<VkSurfaceFormatKHR>& formats);
    VkPresentModeKHR   choose_present_mode  (const std::vector<VkPresentModeKHR>&   modes);
    VkExtent2D         choose_extent        (const VkSurfaceCapabilitiesKHR&        caps);
};

} // namespace Engine
