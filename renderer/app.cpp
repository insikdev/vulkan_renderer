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
    CreateDescriptorSetLayout();
    CreatePipeline();
    p_wsi->GetSwapchain()->CreateFrameBuffers(renderPass);
    CreateCommandPool();
    CreateCommandBuffer();
    CreateSyncObjects();
    CreateMesh();
    CreateUniformBuffer();
    CreateDescriptorPool();
    CreateDescriptorSets();
}

App::~App()
{
    delete m_mesh;
    vkDestroyDescriptorPool(p_wsi->GetDevice()->GetHandle(), descriptorPool, nullptr);

    p_wsi->GetDevice()->DestroyBuffer(globalUBO);
    vkDestroySemaphore(p_wsi->GetDevice()->GetHandle(), imageAvailableSemaphore, nullptr);
    vkDestroySemaphore(p_wsi->GetDevice()->GetHandle(), renderFinishedSemaphore, nullptr);
    vkDestroyFence(p_wsi->GetDevice()->GetHandle(), inFlightFence, nullptr);

    vkDestroyCommandPool(p_wsi->GetDevice()->GetHandle(), commandPool, nullptr);
    vkDestroyDescriptorSetLayout(p_wsi->GetDevice()->GetHandle(), descriptorSetLayout, nullptr);
    vkDestroyPipeline(p_wsi->GetDevice()->GetHandle(), graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(p_wsi->GetDevice()->GetHandle(), pipelineLayout, nullptr);
    vkDestroyRenderPass(p_wsi->GetDevice()->GetHandle(), renderPass, nullptr);

    delete p_wsi;
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void App::Run(void)
{
    while (glfwWindowShouldClose(m_window) != GL_TRUE) {
        glfwPollEvents();
        Draw();
    }

    vkDeviceWaitIdle(p_wsi->GetDevice()->GetHandle());
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

    p_wsi = new VK::WSI { instanceLayers, instanceExtensions, deviceExtensions, hinstance, hwnd };
}

void App::CreateRenderPass(void)
{
    VkAttachmentDescription colorAttachment {
        .flags {},
        .format { p_wsi->GetSwapchain()->GetFormat().format },
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

    CHECK_VK(vkCreateRenderPass(p_wsi->GetDevice()->GetHandle(), &renderPassCreateInfo, nullptr, &renderPass), "Failed to create render pass.");
}

void App::CreateDescriptorSetLayout(void)
{
    VkDescriptorSetLayoutBinding globalLayoutBinding {
        .binding { 0 },
        .descriptorType { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER },
        .descriptorCount { 1 },
        .stageFlags { VK_SHADER_STAGE_VERTEX_BIT },
        .pImmutableSamplers { nullptr }
    };

    VkDescriptorSetLayoutCreateInfo layoutInfo {
        .sType { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO },
        .pNext { nullptr },
        .flags {},
        .bindingCount { 1 },
        .pBindings { &globalLayoutBinding }
    };

    CHECK_VK(vkCreateDescriptorSetLayout(p_wsi->GetDevice()->GetHandle(), &layoutInfo, nullptr, &descriptorSetLayout), "Failed to create descriptor set layout.");
}

void App::CreatePipeline(void)
{
    const auto& vertexShaderCode = Utils::ReadFile("./shaders/simple.vert.spv");
    const auto& fragmentShaderCode = Utils::ReadFile("./shaders/simple.frag.spv");

    VK::Shader vertexShader { p_wsi->GetDevice()->GetHandle(), vertexShaderCode };
    VK::Shader fragmentShader { p_wsi->GetDevice()->GetHandle(), fragmentShaderCode };

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

    VkVertexInputBindingDescription bindingDescription {
        .binding { 0 },
        .stride { sizeof(Vertex) },
        .inputRate { VK_VERTEX_INPUT_RATE_VERTEX }
    };

    VkVertexInputAttributeDescription pos {
        .location { 0 },
        .binding { 0 },
        .format { VK_FORMAT_R32G32_SFLOAT },
        .offset { offsetof(Vertex, pos) },
    };

    VkVertexInputAttributeDescription color {
        .location { 1 },
        .binding { 0 },
        .format { VK_FORMAT_R32G32B32_SFLOAT },
        .offset { offsetof(Vertex, color) },
    };

    std::vector<VkVertexInputAttributeDescription> attributeDescriptions { pos, color };

    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo {
        .sType { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO },
        .pNext { nullptr },
        .flags {},
        .vertexBindingDescriptionCount { 1 },
        .pVertexBindingDescriptions { &bindingDescription },
        .vertexAttributeDescriptionCount { static_cast<uint32_t>(attributeDescriptions.size()) },
        .pVertexAttributeDescriptions { attributeDescriptions.data() }
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
        .width { static_cast<float>(p_wsi->GetSwapchain()->GetExtent().width) },
        .height { static_cast<float>(p_wsi->GetSwapchain()->GetExtent().height) },
        .minDepth { 0.0f },
        .maxDepth { 1.0f }
    };

    VkRect2D scissor {
        .offset { 0, 0 },
        .extent { p_wsi->GetSwapchain()->GetExtent() }
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
        .setLayoutCount { 1 },
        .pSetLayouts { &descriptorSetLayout },
        .pushConstantRangeCount { 0 },
        .pPushConstantRanges { nullptr },
    };

    CHECK_VK(vkCreatePipelineLayout(p_wsi->GetDevice()->GetHandle(), &pipelineLayoutCreateInfo, nullptr, &pipelineLayout), "Failed to create pipeline layout.");

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

    CHECK_VK(vkCreateGraphicsPipelines(p_wsi->GetDevice()->GetHandle(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline), "Failed to create pipeline");
}

void App::CreateCommandPool(void)
{
    VkCommandPoolCreateInfo commandPoolCreateInfo {
        .sType { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO },
        .pNext { nullptr },
        .flags { VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT },
        .queueFamilyIndex { p_wsi->GetDevice()->GetGraphicQueueFamilyIndex() }
    };

    CHECK_VK(vkCreateCommandPool(p_wsi->GetDevice()->GetHandle(), &commandPoolCreateInfo, nullptr, &commandPool), "Failed to create command pool");
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

    CHECK_VK(vkAllocateCommandBuffers(p_wsi->GetDevice()->GetHandle(), &allocInfo, &commandBuffer), "Failed to create command bufffer.");
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

    CHECK_VK(vkCreateSemaphore(p_wsi->GetDevice()->GetHandle(), &semaphoreInfo, nullptr, &imageAvailableSemaphore), "");
    CHECK_VK(vkCreateSemaphore(p_wsi->GetDevice()->GetHandle(), &semaphoreInfo, nullptr, &renderFinishedSemaphore), "");
    CHECK_VK(vkCreateFence(p_wsi->GetDevice()->GetHandle(), &fenceInfo, nullptr, &inFlightFence), "");
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
        .extent { p_wsi->GetSwapchain()->GetExtent() }
    };

    VkClearValue clearColor = { { { 0.0f, 0.0f, 0.0f, 1.0f } } };

    VkRenderPassBeginInfo renderPassInfo {
        .sType { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO },
        .pNext { nullptr },
        .renderPass { renderPass },
        .framebuffer { p_wsi->GetSwapchain()->GetFrameBuffer(imageIndex) },
        .renderArea { renderArea },
        .clearValueCount { 1 },
        .pClearValues { &clearColor }
    };

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

    VkViewport viewport {
        .x { 0.0f },
        .y { 0.0f },
        .width { static_cast<float>(p_wsi->GetSwapchain()->GetExtent().width) },
        .height { static_cast<float>(p_wsi->GetSwapchain()->GetExtent().height) },
        .minDepth { 0.0f },
        .maxDepth { 1.0f }
    };

    VkRect2D scissor {
        .offset { 0, 0 },
        .extent { p_wsi->GetSwapchain()->GetExtent() }
    };

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    /**/
    GlobalUniformData ubo {
        .view { glm::mat4(1.0f) },
        .proj { glm::mat4(1.0f) }
    };

    p_wsi->GetDevice()->CopyDataToDevice(globalUBO.allocation, &ubo, sizeof(GlobalUniformData));
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[0], 0, nullptr);
    // memcpy(globalUBO.allocation->(), &ubo, sizeof(GlobalUniformData));
    /**/

    m_mesh->Draw(commandBuffer);

    vkCmdEndRenderPass(commandBuffer);

    CHECK_VK(vkEndCommandBuffer(commandBuffer), "Failed to record command buffer.");
}

void App::CreateMesh(void)
{
    std::vector<Vertex> vertices = {
        { { -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
        { { 0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f } },
        { { 0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f } },
        { { -0.5f, 0.5f }, { 1.0f, 1.0f, 1.0f } }
    };

    std::vector<uint32_t> indices = { 0, 1, 2, 2, 3, 0 };

    m_mesh = new Mesh { p_wsi->GetDevice(), vertices, indices };
}

void App::CreateUniformBuffer(void)
{
    VkDeviceSize bufferSize = sizeof(GlobalUniformData);
    VkBufferUsageFlags bufferUsage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    VmaAllocationCreateFlags allocationFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

    globalUBO = p_wsi->GetDevice()->CreateBuffer(bufferSize, bufferUsage, allocationFlags);
}

void App::CreateDescriptorPool(void)
{
    VkDescriptorPoolSize poolSize {
        .type { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER },
        .descriptorCount { 1 }
    };

    VkDescriptorPoolCreateInfo poolInfo {
        .sType { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO },
        .pNext { nullptr },
        .flags {},
        .maxSets { 1 },
        .poolSizeCount { 1 },
        .pPoolSizes { &poolSize }
    };

    CHECK_VK(vkCreateDescriptorPool(p_wsi->GetDevice()->GetHandle(), &poolInfo, nullptr, &descriptorPool), "Failed to create descriptor pool.");
}

void App::CreateDescriptorSets(void)
{
    descriptorSets.resize(1);
    std::vector<VkDescriptorSetLayout> layouts(1, descriptorSetLayout);

    VkDescriptorSetAllocateInfo allocInfo {
        .sType { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO },
        .pNext {},
        .descriptorPool { descriptorPool },
        .descriptorSetCount { 1 },
        .pSetLayouts { layouts.data() }
    };

    CHECK_VK(vkAllocateDescriptorSets(p_wsi->GetDevice()->GetHandle(), &allocInfo, descriptorSets.data()), "Failed to allocate descriptor set.");

    VkDescriptorBufferInfo bufferInfo {
        .buffer { globalUBO.handle },
        .offset {},
        .range { sizeof(GlobalUniformData) }
    };

    VkWriteDescriptorSet descriptorWrite {
        .sType { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET },
        .pNext { nullptr },
        .dstSet { descriptorSets[0] },
        .dstBinding {},
        .dstArrayElement {},
        .descriptorCount { 1 },
        .descriptorType { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER },
        .pImageInfo {},
        .pBufferInfo { &bufferInfo },
        .pTexelBufferView {}
    };

    vkUpdateDescriptorSets(p_wsi->GetDevice()->GetHandle(), 1, &descriptorWrite, 0, nullptr);
}

void App::Draw(void)
{
    vkWaitForFences(p_wsi->GetDevice()->GetHandle(), 1, &inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(p_wsi->GetDevice()->GetHandle(), 1, &inFlightFence);
    uint32_t imageIndex;
    vkAcquireNextImageKHR(p_wsi->GetDevice()->GetHandle(), p_wsi->GetSwapchain()->GetHandle(), UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

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

    CHECK_VK(vkQueueSubmit(p_wsi->GetDevice()->GetGrahpicsQueue(), 1, &submitInfo, inFlightFence), "Failed to submit command buffer.");

    VkSwapchainKHR swapChains[] = { p_wsi->GetSwapchain()->GetHandle() };

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

    CHECK_VK(vkQueuePresentKHR(p_wsi->GetDevice()->GetPresentQueue(), &presentInfo), "Failed to present image.");
}
