#include "buffer.h"

namespace Engine {

// -----------------------------------------------------------------------------
// Vertex
// -----------------------------------------------------------------------------

VkVertexInputBindingDescription Vertex::getBindingDescription()
{
    return {
        .binding   = 0,
        .stride    = sizeof(Vertex),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
    };
}

std::array<VkVertexInputAttributeDescription, 3> Vertex::getAttributeDescriptions()
{
    return {{
        { .location = 0, .binding = 0, .format = VK_FORMAT_R32G32B32_SFLOAT, .offset = offsetof(Vertex, pos)   },
        { .location = 1, .binding = 0, .format = VK_FORMAT_R32G32B32_SFLOAT, .offset = offsetof(Vertex, color) },
        { .location = 2, .binding = 0, .format = VK_FORMAT_R32G32_SFLOAT,    .offset = offsetof(Vertex, uv)    },
    }};
}

// -----------------------------------------------------------------------------
// Memory helper
// -----------------------------------------------------------------------------

u32 findMemoryType(VkPhysicalDevice physical, u32 typeFilter,
                   VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProps;
    vkGetPhysicalDeviceMemoryProperties(physical, &memProps);

    for (u32 i = 0; i < memProps.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) &&
            (memProps.memoryTypes[i].propertyFlags & properties) == properties)
            return i;
    }

    throw std::runtime_error("failed to find suitable memory type");
}

// -----------------------------------------------------------------------------
// Buffer
// -----------------------------------------------------------------------------

void Buffer::create(VkPhysicalDevice physical, VkDevice device,
                    VkDeviceSize size, VkBufferUsageFlags usage,
                    VkMemoryPropertyFlags properties)
{
    m_device = device;
    m_size   = size;

    const VkBufferCreateInfo bufferInfo {
        .sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size        = size,
        .usage       = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    if (vkCreateBuffer(m_device, &bufferInfo, nullptr, &m_handle) != VK_SUCCESS)
        throw std::runtime_error("failed to create buffer");

    VkMemoryRequirements memReqs;
    vkGetBufferMemoryRequirements(m_device, m_handle, &memReqs);

    const VkMemoryAllocateInfo allocInfo {
        .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize  = memReqs.size,
        .memoryTypeIndex = findMemoryType(physical, memReqs.memoryTypeBits, properties),
    };

    if (vkAllocateMemory(m_device, &allocInfo, nullptr, &m_memory) != VK_SUCCESS)
        throw std::runtime_error("failed to allocate buffer memory");

    vkBindBufferMemory(m_device, m_handle, m_memory, 0);
}

void Buffer::destroy()
{
    if (m_device == VK_NULL_HANDLE) return;

    if (m_handle != VK_NULL_HANDLE) {
        vkDestroyBuffer(m_device, m_handle, nullptr);
        m_handle = VK_NULL_HANDLE;
    }
    if (m_memory != VK_NULL_HANDLE) {
        vkFreeMemory(m_device, m_memory, nullptr);
        m_memory = VK_NULL_HANDLE;
    }

    m_size   = 0;
    m_device = VK_NULL_HANDLE;
}

// -----------------------------------------------------------------------------
// Mesh
// -----------------------------------------------------------------------------

void Mesh::copy(VkDevice device, VkCommandPool pool, VkQueue queue,
                VkBuffer src, VkBuffer dst, VkDeviceSize size)
{
    const VkCommandBufferAllocateInfo allocInfo {
        .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool        = pool,
        .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };

    VkCommandBuffer cmd;
    vkAllocateCommandBuffers(device, &allocInfo, &cmd);

    const VkCommandBufferBeginInfo beginInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };
    vkBeginCommandBuffer(cmd, &beginInfo);

    const VkBufferCopy region { .size = size };
    vkCmdCopyBuffer(cmd, src, dst, 1, &region);
    vkEndCommandBuffer(cmd);

    const VkSubmitInfo submitInfo {
        .sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers    = &cmd,
    };
    vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue);

    vkFreeCommandBuffers(device, pool, 1, &cmd);
}

void Mesh::create(VkPhysicalDevice physical, VkDevice device,
                  VkCommandPool pool, VkQueue queue,
                  const std::vector<Vertex>& vertices,
                  const std::vector<u32>& indices)
{
    m_physical = physical;
    m_device   = device;

    // vertex
    {
        const VkDeviceSize size = sizeof(Vertex) * vertices.size();

        Buffer staging;
        staging.create(physical, device, size,
                       VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        void* data;
        vkMapMemory(device, staging.memory(), 0, size, 0, &data);
        memcpy(data, vertices.data(), size);
        vkUnmapMemory(device, staging.memory());

        m_vertex_buffer.create(physical, device, size,
                               VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        copy(device, pool, queue, staging.handle(), m_vertex_buffer.handle(), size);
    }

    // index
    {
        const VkDeviceSize size = sizeof(u32) * indices.size();

        Buffer staging;
        staging.create(physical, device, size,
                       VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        void* data;
        vkMapMemory(device, staging.memory(), 0, size, 0, &data);
        memcpy(data, indices.data(), size);
        vkUnmapMemory(device, staging.memory());

        m_index_buffer.create(physical, device, size,
                              VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        copy(device, pool, queue, staging.handle(), m_index_buffer.handle(), size);
    }

    m_vertex_count = static_cast<u32>(vertices.size());
    m_index_count  = static_cast<u32>(indices.size());

    std::print("[mesh] uploaded ({} verts, {} indices)\n", m_vertex_count, m_index_count);
}

void Mesh::destroy()
{
    m_vertex_buffer.destroy();
    m_index_buffer.destroy();
    m_vertex_count = 0;
    m_index_count  = 0;
    m_device       = VK_NULL_HANDLE;
    m_physical     = VK_NULL_HANDLE;
}

} // namespace Engine
