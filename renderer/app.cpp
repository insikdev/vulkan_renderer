#include "pch.h"
#include "app.h"
#include "utils.h"

App::App(uint32_t width, uint32_t height)
    : m_width { width }
    , m_height { height }
{
    CreateGLFW();
    CreateWSI();
    CreateRenderPass();
    CreatePipeline();
    CreateFrameBuffers();
    CreateCommandPool();
    CreateCommandBuffer();
    CreateSyncObjects();
}

App::~App()
{
    vkDestroySemaphore(p_wsi->GetDevice(), imageAvailableSemaphore, nullptr);
    vkDestroySemaphore(p_wsi->GetDevice(), renderFinishedSemaphore, nullptr);
    vkDestroyFence(p_wsi->GetDevice(), inFlightFence, nullptr);

    vkDestroyCommandPool(p_wsi->GetDevice(), commandPool, nullptr);
    for (auto framebuffer : swapChainFramebuffers) {
        vkDestroyFramebuffer(p_wsi->GetDevice(), framebuffer, nullptr);
    }

    vkDestroyPipeline(p_wsi->GetDevice(), graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(p_wsi->GetDevice(), pipelineLayout, nullptr);
    vkDestroyRenderPass(p_wsi->GetDevice(), renderPass, nullptr);
    p_wsi.reset();

    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void App::Run(void)
{
    while (glfwWindowShouldClose(m_window) != GL_TRUE) {
        glfwPollEvents();
        Draw();
    }

    vkDeviceWaitIdle(p_wsi->GetDevice());
}

void App::CreateGLFW(void)
{
    if (glfwInit() == GLFW_FALSE) {
        throw std::exception("Failed to initialize app.");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_window = glfwCreateWindow(m_width, m_height, "Vulkan", nullptr, nullptr);

    if (m_window == nullptr) {
        throw std::exception("Failed to create window.");
    }
}

void App::CreateWSI(void)
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> instanceLayers {};
    std::vector<const char*> instanceExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    std::vector<const char*> deviceExtensions { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    HINSTANCE hinstance = GetModuleHandle(nullptr);
    HWND hwnd = glfwGetWin32Window(m_window);

    p_wsi = VK::WSI::CreateWin32(instanceLayers, instanceExtensions, deviceExtensions, hinstance, hwnd);
}

void App::CreateRenderPass(void)
{
    VkAttachmentDescription colorAttachment {
        .flags {},
        .format { p_wsi->GetFormat() },
        .samples { VK_SAMPLE_COUNT_1_BIT },
        .loadOp { VK_ATTACHMENT_LOAD_OP_CLEAR },
        .storeOp { VK_ATTACHMENT_STORE_OP_STORE },
        .stencilLoadOp { VK_ATTACHMENT_LOAD_OP_DONT_CARE },
        .stencilStoreOp { VK_ATTACHMENT_STORE_OP_DONT_CARE },
        .initialLayout { VK_IMAGE_LAYOUT_UNDEFINED },
        .finalLayout { VK_IMAGE_LAYOUT_PRESENT_SRC_KHR },
    };

    VkAttachmentReference colorAttachmentRef {
        .attachment { 0 },
        .layout { VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL }
    };

    VkSubpassDescription subpass {
        .flags {},
        .pipelineBindPoint { VK_PIPELINE_BIND_POINT_GRAPHICS },
        .inputAttachmentCount {},
        .pInputAttachments {},
        .colorAttachmentCount { 1 },
        .pColorAttachments { &colorAttachmentRef },
        .pResolveAttachments {},
        .pDepthStencilAttachment {},
        .preserveAttachmentCount {},
        .pPreserveAttachments {}
    };

    VkSubpassDependency dependency {
        .srcSubpass { VK_SUBPASS_EXTERNAL },
        .dstSubpass { 0 },
        .srcStageMask { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT },
        .dstStageMask { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT },
        .srcAccessMask { 0 },
        .dstAccessMask { VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT },
        .dependencyFlags {}
    };

    VkRenderPassCreateInfo renderPassCreateInfo {
        .sType { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO },
        .pNext { nullptr },
        .flags {},
        .attachmentCount { 1 },
        .pAttachments { &colorAttachment },
        .subpassCount { 1 },
        .pSubpasses { &subpass },
        .dependencyCount { 1 },
        .pDependencies { &dependency }
    };

    CHECK_VK(vkCreateRenderPass(p_wsi->GetDevice(), &renderPassCreateInfo, nullptr, &renderPass), "Failed to create render pass.");
}

void App::CreatePipeline(void)
{
    const auto& vertexShaderCode = Utils::ReadFile("./shaders/simple.vert.spv");
    const auto& fragmentShaderCode = Utils::ReadFile("./shaders/simple.frag.spv");

    VK::Shader vertexShader { p_wsi->GetDevice(), vertexShaderCode };
    VK::Shader fragmentShader { p_wsi->GetDevice(), fragmentShaderCode };

    VkPipelineShaderStageCreateInfo vertexShaderStageInfo {
        .sType { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO },
        .pNext { nullptr },
        .flags {},
        .stage { VK_SHADER_STAGE_VERTEX_BIT },
        .module { vertexShader.GetHandle() },
        .pName { "main" },
        .pSpecializationInfo {}
    };

    VkPipelineShaderStageCreateInfo fragmentShaderStageInfo {
        .sType { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO },
        .pNext { nullptr },
        .flags {},
        .stage { VK_SHADER_STAGE_FRAGMENT_BIT },
        .module { fragmentShader.GetHandle() },
        .pName { "main" },
        .pSpecializationInfo {}
    };

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertexShaderStageInfo, fragmentShaderStageInfo };

    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo {
        .sType { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO },
        .pNext { nullptr },
        .flags {},
        .dynamicStateCount { static_cast<uint32_t>(dynamicStates.size()) },
        .pDynamicStates { dynamicStates.data() }
    };

    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo {
        .sType { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO },
        .pNext { nullptr },
        .flags {},
        .vertexBindingDescriptionCount {},
        .pVertexBindingDescriptions {},
        .vertexAttributeDescriptionCount {},
        .pVertexAttributeDescriptions {}
    };

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo {
        .sType { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO },
        .pNext { nullptr },
        .flags {},
        .topology { VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST },
        .primitiveRestartEnable { VK_FALSE }
    };

    VkViewport viewport {
        .x { 0.0f },
        .y { 0.0f },
        .width { static_cast<float>(p_wsi->GetExtent().width) },
        .height { static_cast<float>(p_wsi->GetExtent().height) },
        .minDepth { 0.0f },
        .maxDepth { 1.0f }
    };

    VkRect2D scissor {
        .offset { 0, 0 },
        .extent { p_wsi->GetExtent() }
    };

    VkPipelineViewportStateCreateInfo viewportStateCreateInfo {
        .sType { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO },
        .pNext { nullptr },
        .flags {},
        .viewportCount { 1 },
        .pViewports { &viewport },
        .scissorCount { 1 },
        .pScissors { &scissor }
    };

    VkPipelineRasterizationStateCreateInfo rasterizerStateCreateInfo {
        .sType { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO },
        .pNext { nullptr },
        .flags {},
        .depthClampEnable { VK_FALSE },
        .rasterizerDiscardEnable { VK_FALSE },
        .polygonMode { VK_POLYGON_MODE_FILL },
        .cullMode { VK_CULL_MODE_BACK_BIT },
        .frontFace { VK_FRONT_FACE_CLOCKWISE },
        .depthBiasEnable { VK_FALSE },
        .depthBiasConstantFactor {},
        .depthBiasClamp {},
        .depthBiasSlopeFactor {},
        .lineWidth { 1.0f }
    };

    VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo {
        .sType { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO },
        .pNext { nullptr },
        .flags {},
        .rasterizationSamples { VK_SAMPLE_COUNT_1_BIT },
        .sampleShadingEnable { VK_FALSE },
        .minSampleShading { 1.0f },
        .pSampleMask { nullptr },
        .alphaToCoverageEnable { VK_FALSE },
        .alphaToOneEnable = { VK_FALSE }
    };

    VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo {
        .sType { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO },
        .pNext { nullptr },
        .flags {},
        .depthTestEnable {},
        .depthWriteEnable {},
        .depthCompareOp {},
        .depthBoundsTestEnable {},
        .stencilTestEnable {},
        .front {},
        .back {},
        .minDepthBounds {},
        .maxDepthBounds {},
    };

    VkPipelineColorBlendAttachmentState colorBlendAttachmentState {
        .blendEnable { VK_FALSE },
        .srcColorBlendFactor { VK_BLEND_FACTOR_ONE },
        .dstColorBlendFactor { VK_BLEND_FACTOR_ZERO },
        .colorBlendOp { VK_BLEND_OP_ADD },
        .srcAlphaBlendFactor { VK_BLEND_FACTOR_ONE },
        .dstAlphaBlendFactor { VK_BLEND_FACTOR_ZERO },
        .alphaBlendOp { VK_BLEND_OP_ADD },
        .colorWriteMask { VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT },
    };

    VkPipelineColorBlendStateCreateInfo colorBlendingStateCreateInfo {
        .sType { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO },
        .pNext { nullptr },
        .flags {},
        .logicOpEnable { VK_FALSE },
        .logicOp { VK_LOGIC_OP_COPY },
        .attachmentCount { 1 },
        .pAttachments { &colorBlendAttachmentState },
        .blendConstants { 0.0f, 0.0f, 0.0f, 0.0f }
    };

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo {
        .sType { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO },
        .pNext { nullptr },
        .flags {},
        .setLayoutCount { 0 },
        .pSetLayouts { nullptr },
        .pushConstantRangeCount { 0 },
        .pPushConstantRanges { nullptr },
    };

    CHECK_VK(vkCreatePipelineLayout(p_wsi->GetDevice(), &pipelineLayoutCreateInfo, nullptr, &pipelineLayout), "Failed to create pipeline layout.");

    VkGraphicsPipelineCreateInfo pipelineInfo {
        .sType { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO },
        .pNext { nullptr },
        .flags {},
        .stageCount { 2 },
        .pStages { shaderStages },
        .pVertexInputState { &vertexInputStateCreateInfo },
        .pInputAssemblyState { &inputAssemblyStateCreateInfo },
        .pTessellationState {},
        .pViewportState { &viewportStateCreateInfo },
        .pRasterizationState { &rasterizerStateCreateInfo },
        .pMultisampleState { &multisampleStateCreateInfo },
        .pDepthStencilState { &depthStencilStateCreateInfo },
        .pColorBlendState { &colorBlendingStateCreateInfo },
        .pDynamicState { &dynamicStateCreateInfo },
        .layout { pipelineLayout },
        .renderPass { renderPass },
        .subpass {},
        .basePipelineHandle {},
        .basePipelineIndex {}
    };

    CHECK_VK(vkCreateGraphicsPipelines(p_wsi->GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline), "Failed to create pipeline");
}

void App::CreateFrameBuffers(void)
{
    const auto& imageView = p_wsi->GetImageViews();
    swapChainFramebuffers.resize(imageView.size());

    for (size_t i = 0; i < imageView.size(); i++) {
        VkImageView attachments[] = {
            imageView[i]
        };

        VkFramebufferCreateInfo framebufferInfo {
            .sType { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO },
            .pNext { nullptr },
            .flags {},
            .renderPass { renderPass },
            .attachmentCount { 1 },
            .pAttachments { attachments },
            .width { p_wsi->GetExtent().width },
            .height { p_wsi->GetExtent().height },
            .layers { 1 },
        };

        CHECK_VK(vkCreateFramebuffer(p_wsi->GetDevice(), &framebufferInfo, nullptr, &swapChainFramebuffers[i]), "Failed to create frame buffer.");
    }
}

void App::CreateCommandPool(void)
{
    VkCommandPoolCreateInfo commandPoolCreateInfo {
        .sType { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO },
        .pNext { nullptr },
        .flags { VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT },
        .queueFamilyIndex { 0 }
    };

    CHECK_VK(vkCreateCommandPool(p_wsi->GetDevice(), &commandPoolCreateInfo, nullptr, &commandPool), "Failed to create command pool");
}

void App::CreateCommandBuffer(void)
{
    VkCommandBufferAllocateInfo allocInfo {
        .sType { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO },
        .pNext { nullptr },
        .commandPool { commandPool },
        .level { VK_COMMAND_BUFFER_LEVEL_PRIMARY },
        .commandBufferCount { 1 },
    };

    CHECK_VK(vkAllocateCommandBuffers(p_wsi->GetDevice(), &allocInfo, &commandBuffer), "Failed to create command bufffer.");
}

void App::CreateSyncObjects(void)
{
    VkSemaphoreCreateInfo semaphoreInfo {
        .sType { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO },
        .pNext { nullptr },
        .flags {}
    };

    VkFenceCreateInfo fenceInfo {
        .sType { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO },
        .pNext { nullptr },
        .flags { VK_FENCE_CREATE_SIGNALED_BIT }
    };

    CHECK_VK(vkCreateSemaphore(p_wsi->GetDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphore), "");
    CHECK_VK(vkCreateSemaphore(p_wsi->GetDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphore), "");
    CHECK_VK(vkCreateFence(p_wsi->GetDevice(), &fenceInfo, nullptr, &inFlightFence), "");
}

void App::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
    VkCommandBufferBeginInfo beginInfo {
        .sType { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO },
        .pNext { nullptr },
        .flags {},
        .pInheritanceInfo {}
    };

    CHECK_VK(vkBeginCommandBuffer(commandBuffer, &beginInfo), "Failed to begin recording command buffer.");

    VkRect2D renderArea {
        .offset { 0, 0 },
        .extent { p_wsi->GetExtent() }
    };

    VkClearValue clearColor = { { { 0.0f, 0.0f, 0.0f, 1.0f } } };

    VkRenderPassBeginInfo renderPassInfo {
        .sType { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO },
        .pNext { nullptr },
        .renderPass { renderPass },
        .framebuffer { swapChainFramebuffers[imageIndex] },
        .renderArea { renderArea },
        .clearValueCount { 1 },
        .pClearValues { &clearColor }
    };

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

    VkViewport viewport {
        .x { 0.0f },
        .y { 0.0f },
        .width { static_cast<float>(p_wsi->GetExtent().width) },
        .height { static_cast<float>(p_wsi->GetExtent().height) },
        .minDepth { 0.0f },
        .maxDepth { 1.0f }
    };

    VkRect2D scissor {
        .offset { 0, 0 },
        .extent { p_wsi->GetExtent() }
    };

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    vkCmdDraw(commandBuffer, 3, 1, 0, 0);
    vkCmdEndRenderPass(commandBuffer);

    CHECK_VK(vkEndCommandBuffer(commandBuffer), "Failed to record command buffer.");
}

void App::Draw(void)
{
    vkWaitForFences(p_wsi->GetDevice(), 1, &inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(p_wsi->GetDevice(), 1, &inFlightFence);
    uint32_t imageIndex;
    vkAcquireNextImageKHR(p_wsi->GetDevice(), p_wsi->GetSwapchain(), UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

    vkResetCommandBuffer(commandBuffer, 0);
    recordCommandBuffer(commandBuffer, imageIndex);

    VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };

    VkSubmitInfo submitInfo {
        .sType { VK_STRUCTURE_TYPE_SUBMIT_INFO },
        .pNext { nullptr },
        .waitSemaphoreCount { 1 },
        .pWaitSemaphores { waitSemaphores },
        .pWaitDstStageMask { waitStages },
        .commandBufferCount { 1 },
        .pCommandBuffers { &commandBuffer },
        .signalSemaphoreCount { 1 },
        .pSignalSemaphores { signalSemaphores }
    };

    CHECK_VK(vkQueueSubmit(p_wsi->GetGrahpicsQueue(), 1, &submitInfo, inFlightFence), "Failed to submit command buffer.");

    VkSwapchainKHR swapChains[] = { p_wsi->GetSwapchain() };

    VkPresentInfoKHR presentInfo {
        .sType { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR },
        .pNext { nullptr },
        .waitSemaphoreCount { 1 },
        .pWaitSemaphores { signalSemaphores },
        .swapchainCount { 1 },
        .pSwapchains { swapChains },
        .pImageIndices { &imageIndex },
        .pResults { nullptr },
    };

    CHECK_VK(vkQueuePresentKHR(p_wsi->GetPresentQueue(), &presentInfo), "Failed to present image.");
}
