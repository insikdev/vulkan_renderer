#pragma once

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
    void CreatePipeline(void);
    void CreateFrameBuffers(void);
    void CreateCommandPool(void);
    void CreateCommandBuffer(void);
    void CreateSyncObjects(void);
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

    void Draw(void);

private:
    uint32_t m_width;
    uint32_t m_height;
    GLFWwindow* m_window;

private:
    std::unique_ptr<VK::WSI> p_wsi;
    VkRenderPass renderPass;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    std::vector<VkFramebuffer> swapChainFramebuffers;
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;
};
