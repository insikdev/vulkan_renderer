#pragma once

#include "mesh.h"

class App {
public:
    App(uint32_t width, uint32_t height);
    ~App();

public:
    void Run(void);

private:
    void CreateGLFW(void);
    void CreateWSI(void);
    void CreateRenderPass(void);
    void CreateDescriptorSetLayout(void);
    void CreatePipeline(void);
    void CreateCommandPool(void);
    void CreateCommandBuffer();
    void CreateSyncObjects(void);
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void CreateMesh(void);
    void CreateUniformBuffer(void);
    void CreateDescriptorPool(void);
    void CreateDescriptorSets(void);
    void CreateDepthResources(void);
    void CreateTexture(void);

    void CreateSampler();

    void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    void Update(void);
    void Render(void);

private:
    uint32_t m_width;
    uint32_t m_height;
    GLFWwindow* m_window;

private:
    VK::WSI* p_wsi;
    VkRenderPass renderPass;
    VkDescriptorSetLayout descriptorSetLayout;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    VkDescriptorPool descriptorPool;
    // VkCommandPool commandPool;
    VK::CommandPool* commandPool;

private:
    const uint32_t MAX_FRAME = 3;
    std::vector<FrameData> m_frameData { MAX_FRAME };
    uint32_t m_currentFrame {};
    Mesh* m_mesh;
    VK::Image texture;
    VkImageView textureImageView;
    VkSampler textureSampler;

private: // depth
    VK::Image depthImage;
    VkImageView depthImageView;
};
