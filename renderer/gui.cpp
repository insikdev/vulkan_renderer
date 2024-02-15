#include "pch.h"
#include "gui.h"

GUI::GUI(GLFWwindow* pWindow, VK::WSI* pWSI, VkRenderPass& renderPass)
    : p_wsi { pWSI }
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForVulkan(pWindow, true);

    std::vector<VkDescriptorPoolSize> pool_sizes = {
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 },
    };

    m_descriptorPool = p_wsi->GetDevice()->CreateDescriptorPool(1, pool_sizes, VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT);
    m_commandPool = p_wsi->GetDevice()->CreateCommandPool();

    ImGui_ImplVulkan_InitInfo info {
        .Instance { p_wsi->GetInstance()->GetHandle() },
        .PhysicalDevice { p_wsi->GetDevice()->GetPhysicalDeviceHandle() },
        .Device { p_wsi->GetDevice()->GetHandle() },
        .QueueFamily { p_wsi->GetDevice()->GetGraphicQueueFamilyIndex() },
        .Queue { p_wsi->GetDevice()->GetGrahpicsQueue() },
        .PipelineCache {},
        .DescriptorPool { m_descriptorPool.GetHandle() },
        .Subpass {},
        .MinImageCount { 2 },
        .ImageCount { 3 },
        .MinAllocationSize { 1024 * 1024 }
    };

    ImGui_ImplVulkan_Init(&info, renderPass);
}

GUI::~GUI()
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    m_commandPool.Destroy();
    m_descriptorPool.Destroy();
}

void GUI::Render(const VkCommandBuffer& commandBuffer)
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    //
    bool show { true };
    ImGui::ShowDemoWindow(&show);
    //
    ImGui::Render();

    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
}
