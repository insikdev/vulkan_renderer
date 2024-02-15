#include "pch.h"
#include "gui.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

GUI::GUI(GLFWwindow* pWindow, VkRenderPass& renderPass)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForVulkan(pWindow, true);

    std::vector<VkDescriptorPoolSize> pool_sizes = {
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 },
    };

    // m_descriptorPool = p_wsi->GetDevice()->CreateDescriptorPool(1, pool_sizes, VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT);

    /* ImGui_ImplVulkan_InitInfo info {
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

     ImGui_ImplVulkan_Init(&info, renderPass);*/
}

GUI::~GUI()
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    m_descriptorPool.Destroy();
}

void GUI::Render(const VkCommandBuffer& commandBuffer)
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    {
        ImGui::Begin("GUI", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::SetWindowPos(ImVec2(10, 10));

        ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();

        ImGui::Render();
    }

    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
}
