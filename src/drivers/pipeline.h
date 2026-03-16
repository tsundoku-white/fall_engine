#pragma once
#include "types.h"
#include <fstream>
#include <print>
#include <vector>

namespace Engine {

class Pipeline {
public:
    void create(VkDevice device, VkRenderPass render_pass, VkExtent2D extent);
    void destroy();
    ~Pipeline() { destroy(); }

    VkPipeline       handle()          const { return m_pipeline; }
    VkPipelineLayout layout()          const { return m_pipeline_layout; }

private:
    VkDevice         m_device          = VK_NULL_HANDLE;
    VkPipeline       m_pipeline        = VK_NULL_HANDLE;
    VkPipelineLayout m_pipeline_layout = VK_NULL_HANDLE;

    VkShaderModule createShaderModule(const std::vector<char>& code);
    static std::vector<char> readFile(const std::string& path);
};

} 
