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
    void CreateCommandBuffer(void);
    void CreateSyncObjects(void);
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void CreateMesh(void);
    void CreateUniformBuffer(void);
    void CreateDescriptorPool(void);
    void CreateDescriptorSets(void);

    void Draw(void);

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
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;
    Mesh* m_mesh;
    VK::Buffer globalUBO;
    VkDescriptorPool descriptorPool;
    // VkDescriptorSet descriptorSets;
    std::vector<VkDescriptorSet> descriptorSets;
};
