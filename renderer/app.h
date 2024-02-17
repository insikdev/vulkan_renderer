#pragma once

class Model;
class GUI;

class App {
public:
    App() = default;
    ~App() { Destroy(); }
    App(const App&) = delete;
    App(App&&) = delete;
    App& operator=(const App&) = delete;
    App& operator=(App&&) = delete;

public:
    void Init(uint32_t width, uint32_t height);
    void Destroy(void);
    void Run(void);

private:
    void InitGLFW(uint32_t width, uint32_t height);
    void InitVulkan(void);
    void InitPipeline(void);
    void InitFrameBuffer(void);
    void InitGui(void);
    void InitModel(void);

    void CreateSampler();

    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void Update(void);
    void Render(void);

private:
    uint32_t m_width;
    uint32_t m_height;
    GLFWwindow* m_window;

private: // vulkan object
    VK::Instance m_instance;
    VK::Surface m_surface;
    VK::PhysicalDevice m_physicalDevice;
    VK::Device m_device;
    VK::Queue m_graphicsQueue;
    VK::Swapchain m_swapchain;
    VK::MemoryAllocator m_memoryAllocator;
    VK::CommandPool m_transientCommandPool;
    VK::CommandPool m_resetCommandPool;
    VK::DescriptorPool m_descriptorPool;
    VK::DescriptorPool m_guiDescriptorPool;
    VkRenderPass renderPass { VK_NULL_HANDLE };
    VkDescriptorSetLayout descriptorSetLayout { VK_NULL_HANDLE };
    VkPipelineLayout pipelineLayout { VK_NULL_HANDLE };
    VkPipeline graphicsPipeline { VK_NULL_HANDLE };
    VkPipeline wireGraphicsPipeline { VK_NULL_HANDLE };

private:
    // std::optional<uint32_t> m_graphicsQueueFamilyIndex { std::nullopt };
    VkFormat m_swapchainImageFormat;
    VkExtent2D m_swapchainImageExtent;

private:
    GUI* p_gui;
    GuiOptions guiOptions {};

private:
    static const uint32_t MAX_FRAME = 2;
    FrameData m_frameData[MAX_FRAME];
    uint32_t m_currentFrame {};
    Model* m_model;
    VkSampler textureSampler { VK_NULL_HANDLE };

private: // frame buffer
    std::vector<VK::FrameBuffer> m_frameBuffers;
    std::vector<VK::ImageView> m_colorImages;
    VK::Image depthImage;
    VK::ImageView depthImageView;
};
