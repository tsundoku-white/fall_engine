#pragma once 

#include "platform.h"
#include "vulkan_context.h"
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>
#include <vector>
#include <fstream>

namespace helix {

  class Render 
  {
    public:
      Render(Vulkan_Context &ctx, Platform &platform);
      void pass();
      ~Render();
    private:
      VkDevice m_device                  = VK_NULL_HANDLE;
      VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
      VkSurfaceKHR m_surface             = VK_NULL_HANDLE;
      GLFWwindow *m_window               = nullptr;
      uint32_t m_graphics_family_index   = 0;

      std::vector<VkImageView>   m_image_views;
      VkRenderPass               m_render_pass      = VK_NULL_HANDLE;


      VkSwapchainKHR m_swapchain         = VK_NULL_HANDLE;
      std::vector<VkImage> m_swapchain_images;
      VkFormat m_swapchain_format        = VK_FORMAT_UNDEFINED;
      VkExtent2D m_swapchain_extent      = {};
      VkPipelineLayout m_pipeline_layout = VK_NULL_HANDLE;
      VkPipeline m_graphics_pipeline     = VK_NULL_HANDLE;
      VkCommandPool m_command_pool       = VK_NULL_HANDLE;
      std::vector<VkCommandBuffer> commandBuffers {};
      VkQueue m_graphics_queue           = VK_NULL_HANDLE;
      VkQueue m_present_queue            = VK_NULL_HANDLE;

      uint32_t m_current_frame = 0;

      std::vector<VkSemaphore> imageAvailableSemaphores;
      std::vector<VkSemaphore> renderFinishedSemaphores; 
      std::vector<VkFence> inFlightFences;

      void createSwapchain(Vulkan_Context &ctx);
      void createImageViews();
      void createRenderPass();
      void createGraphicsPipeline();
      void createFramebuffers();
      void createCommandPool();
      void createCommandBuffer();
      void createSyncObjects();

      std::vector<VkFramebuffer> m_swap_chain_framebuffers;

      void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

      VkShaderModule createShaderModule(const std::vector<char>& code);

      VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);
      VkPresentModeKHR   choosePresentMode  (const std::vector<VkPresentModeKHR>&   modes);
      VkExtent2D         chooseExtent       (const VkSurfaceCapabilitiesKHR&        caps);
  };

}
