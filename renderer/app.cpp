#include "pch.h"
#include "app.h"
#include "utils.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define CURRENT_FRAME m_frameData[m_currentFrame]
#define DEVICE p_wsi->GetDevice()
#define SWAPCHAIN p_wsi->GetSwapchain()

App::App(uint32_t width, uint32_t height)
    : m_width { width }
    , m_height { height }
{
    CreateGLFW();
    CreateWSI();
    CreateMesh();
    CreateTexture();
    textureImageView = DEVICE->CreateImageView(texture.handle, VK_FORMAT_R8G8B8A8_SRGB);
    CreateSampler();
    CreateRenderPass();
    CreateDescriptorSetLayout();
    CreatePipeline();
    SWAPCHAIN->CreateFrameBuffers(renderPass);
    CreateCommandPool();
    CreateCommandBuffer();
    CreateSyncObjects();
    CreateUniformBuffer();
    CreateDescriptorPool();
    CreateDescriptorSets();
}

App::~App()
{
    delete m_mesh;
    vkDestroySampler(DEVICE->GetHandle(), textureSampler, nullptr);
    vkDestroyImageView(DEVICE->GetHandle(), textureImageView, nullptr);
    DEVICE->DestroyImage(texture);

    for (uint32_t i = 0; i < MAX_FRAME; i++) {
        DEVICE->DestroyBuffer(m_frameData[i].globalUBO);
        vkDestroySemaphore(DEVICE->GetHandle(), m_frameData[i].imageAvailableSemaphore, nullptr);
        vkDestroySemaphore(DEVICE->GetHandle(), m_frameData[i].renderFinishedSemaphore, nullptr);
        vkDestroyFence(DEVICE->GetHandle(), m_frameData[i].inFlightFence, nullptr);
    }

    vkDestroyDescriptorPool(DEVICE->GetHandle(), descriptorPool, nullptr);
    vkDestroyCommandPool(DEVICE->GetHandle(), commandPool, nullptr);
    vkDestroyDescriptorSetLayout(DEVICE->GetHandle(), descriptorSetLayout, nullptr);
    vkDestroyPipeline(DEVICE->GetHandle(), graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(DEVICE->GetHandle(), pipelineLayout, nullptr);
    vkDestroyRenderPass(DEVICE->GetHandle(), renderPass, nullptr);

    delete p_wsi;
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void App::Run(void)
{
    while (glfwWindowShouldClose(m_window) != GL_TRUE) {
        glfwPollEvents();
        Update();
        Render();
    }

    vkDeviceWaitIdle(DEVICE->GetHandle());
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
        .format { SWAPCHAIN->GetFormat().format },
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

    CHECK_VK(vkCreateRenderPass(DEVICE->GetHandle(), &renderPassCreateInfo, nullptr, &renderPass), "Failed to create render pass.");
}

void App::CreateDescriptorSetLayout(void)
{
    VkDescriptorSetLayoutBinding binding0 { // model
        .binding { 0 },
        .descriptorType { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER },
        .descriptorCount { 1 },
        .stageFlags { VK_SHADER_STAGE_VERTEX_BIT },
        .pImmutableSamplers { nullptr }
    };

    VkDescriptorSetLayoutBinding binding1 { // global
        .binding { 1 },
        .descriptorType { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER },
        .descriptorCount { 1 },
        .stageFlags { VK_SHADER_STAGE_VERTEX_BIT },
        .pImmutableSamplers { nullptr }
    };

    VkDescriptorSetLayoutBinding binding2 { // global
        .binding { 2 },
        .descriptorType { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER },
        .descriptorCount { 1 },
        .stageFlags { VK_SHADER_STAGE_FRAGMENT_BIT },
        .pImmutableSamplers { nullptr }
    };

    std::vector<VkDescriptorSetLayoutBinding> bindings { binding0, binding1, binding2 };

    VkDescriptorSetLayoutCreateInfo layoutInfo {
        .sType { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO },
        .pNext { nullptr },
        .flags {},
        .bindingCount { static_cast<uint32_t>(bindings.size()) },
        .pBindings { bindings.data() }
    };

    CHECK_VK(vkCreateDescriptorSetLayout(DEVICE->GetHandle(), &layoutInfo, nullptr, &descriptorSetLayout), "Failed to create descriptor set layout.");
}

void App::CreatePipeline(void)
{
    const auto& vertexShaderCode = Utils::ReadFile("./shaders/simple.vert.spv");
    const auto& fragmentShaderCode = Utils::ReadFile("./shaders/simple.frag.spv");

    VK::Shader vertexShader { DEVICE->GetHandle(), vertexShaderCode };
    VK::Shader fragmentShader { DEVICE->GetHandle(), fragmentShaderCode };

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

    VkVertexInputAttributeDescription uv {
        .location { 2 },
        .binding { 0 },
        .format { VK_FORMAT_R32G32_SFLOAT },
        .offset { offsetof(Vertex, uv) },
    };

    std::vector<VkVertexInputAttributeDescription> attributeDescriptions { pos, color, uv };

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
        .width { static_cast<float>(SWAPCHAIN->GetExtent().width) },
        .height { static_cast<float>(SWAPCHAIN->GetExtent().height) },
        .minDepth { 0.0f },
        .maxDepth { 1.0f }
    };

    VkRect2D scissor {
        .offset { 0, 0 },
        .extent { SWAPCHAIN->GetExtent() }
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

    std::vector<VkDescriptorSetLayout> descriptorSetLayouts { descriptorSetLayout };

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo {
        .sType { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO },
        .pNext { nullptr },
        .flags {},
        .setLayoutCount { static_cast<uint32_t>(descriptorSetLayouts.size()) },
        .pSetLayouts { descriptorSetLayouts.data() },
        .pushConstantRangeCount { 0 },
        .pPushConstantRanges { nullptr },
    };

    CHECK_VK(vkCreatePipelineLayout(DEVICE->GetHandle(), &pipelineLayoutCreateInfo, nullptr, &pipelineLayout), "Failed to create pipeline layout.");

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

    CHECK_VK(vkCreateGraphicsPipelines(DEVICE->GetHandle(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline), "Failed to create pipeline");
}

void App::CreateCommandPool(void)
{
    VkCommandPoolCreateInfo commandPoolCreateInfo {
        .sType { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO },
        .pNext { nullptr },
        .flags { VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT },
        .queueFamilyIndex { DEVICE->GetGraphicQueueFamilyIndex() }
    };

    CHECK_VK(vkCreateCommandPool(DEVICE->GetHandle(), &commandPoolCreateInfo, nullptr, &commandPool), "Failed to create command pool");
}

void App::CreateCommandBuffer()
{
    VkCommandBufferAllocateInfo allocInfo {
        .sType { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO },
        .pNext { nullptr },
        .commandPool { commandPool },
        .level { VK_COMMAND_BUFFER_LEVEL_PRIMARY },
        .commandBufferCount { 1 },
    };

    for (uint32_t i = 0; i < MAX_FRAME; i++) {
        CHECK_VK(vkAllocateCommandBuffers(DEVICE->GetHandle(), &allocInfo, &m_frameData[i].commandBuffer), "Failed to create command bufffer.");
    }
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

    for (uint32_t i = 0; i < MAX_FRAME; i++) {
        CHECK_VK(vkCreateSemaphore(DEVICE->GetHandle(), &semaphoreInfo, nullptr, &m_frameData[i].imageAvailableSemaphore), "");
        CHECK_VK(vkCreateSemaphore(DEVICE->GetHandle(), &semaphoreInfo, nullptr, &m_frameData[i].renderFinishedSemaphore), "");
        CHECK_VK(vkCreateFence(DEVICE->GetHandle(), &fenceInfo, nullptr, &m_frameData[i].inFlightFence), "");
    }
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
        .extent { SWAPCHAIN->GetExtent() }
    };

    VkClearValue clearColor = { { { 0.0f, 0.0f, 0.0f, 1.0f } } };

    VkRenderPassBeginInfo renderPassInfo {
        .sType { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO },
        .pNext { nullptr },
        .renderPass { renderPass },
        .framebuffer { SWAPCHAIN->GetFrameBuffer(imageIndex) },
        .renderArea { renderArea },
        .clearValueCount { 1 },
        .pClearValues { &clearColor }
    };

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

    VkViewport viewport {
        .x { 0.0f },
        .y { 0.0f },
        .width { static_cast<float>(SWAPCHAIN->GetExtent().width) },
        .height { static_cast<float>(SWAPCHAIN->GetExtent().height) },
        .minDepth { 0.0f },
        .maxDepth { 1.0f }
    };

    VkRect2D scissor {
        .offset { 0, 0 },
        .extent { SWAPCHAIN->GetExtent() }
    };

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &CURRENT_FRAME.descriptorSets[0], 0, nullptr);

    m_mesh->Draw(commandBuffer);

    vkCmdEndRenderPass(commandBuffer);

    CHECK_VK(vkEndCommandBuffer(commandBuffer), "Failed to record command buffer.");
}

void App::CreateMesh(void)
{
    std::vector<Vertex> vertices = {
        { { -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } },
        { { 0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f } },
        { { 0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } },
        { { -0.5f, 0.5f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f } }
    };

    std::vector<uint32_t> indices = { 0, 1, 2, 2, 3, 0 };

    m_mesh = new Mesh { DEVICE, vertices, indices };
}

void App::CreateUniformBuffer(void)
{
    VkDeviceSize bufferSize = sizeof(GlobalUniformData);
    VkBufferUsageFlags bufferUsage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    VmaAllocationCreateFlags allocationFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

    for (uint32_t i = 0; i < MAX_FRAME; i++) {
        m_frameData[i].globalUBO = DEVICE->CreateBuffer(bufferSize, bufferUsage, allocationFlags);
    }
}

void App::CreateDescriptorPool(void)
{
    std::vector<VkDescriptorPoolSize> pools {
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 },
    };

    for (auto& p : pools) {
        p.descriptorCount *= MAX_FRAME;
    }

    VkDescriptorPoolCreateInfo poolInfo {
        .sType { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO },
        .pNext { nullptr },
        .flags {},
        .maxSets { 1 * MAX_FRAME },
        .poolSizeCount { static_cast<uint32_t>(pools.size()) },
        .pPoolSizes { pools.data() }
    };

    CHECK_VK(vkCreateDescriptorPool(DEVICE->GetHandle(), &poolInfo, nullptr, &descriptorPool), "Failed to create descriptor pool.");
}

void App::CreateDescriptorSets(void)
{
    VkDescriptorSetAllocateInfo allocInfo {
        .sType { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO },
        .pNext {},
        .descriptorPool { descriptorPool },
        .descriptorSetCount { 1 },
        .pSetLayouts { &descriptorSetLayout }
    };

    for (uint32_t i = 0; i < MAX_FRAME; i++) {
        m_frameData[i].descriptorSets.resize(1);
        CHECK_VK(vkAllocateDescriptorSets(DEVICE->GetHandle(), &allocInfo, m_frameData[i].descriptorSets.data()), "Failed to allocate descriptor set.");

        VkDescriptorBufferInfo binding0 {
            .buffer { m_mesh->GetUniformBuffer() },
            .offset {},
            .range { sizeof(MeshUniformData) }
        };

        VkDescriptorBufferInfo binding1 {
            .buffer { m_frameData[i].globalUBO.handle },
            .offset {},
            .range { sizeof(GlobalUniformData) }
        };

        VkDescriptorImageInfo binding2 {
            .sampler { textureSampler },
            .imageView { textureImageView },
            .imageLayout { VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL }
        };

        VkWriteDescriptorSet write0 {
            .sType { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET },
            .pNext { nullptr },
            .dstSet { m_frameData[i].descriptorSets[0] },
            .dstBinding { 0 },
            .dstArrayElement {},
            .descriptorCount { 1 },
            .descriptorType { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER },
            .pImageInfo {},
            .pBufferInfo { &binding0 },
            .pTexelBufferView {}
        };

        VkWriteDescriptorSet write1 {
            .sType { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET },
            .pNext { nullptr },
            .dstSet { m_frameData[i].descriptorSets[0] },
            .dstBinding { 1 },
            .dstArrayElement {},
            .descriptorCount { 1 },
            .descriptorType { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER },
            .pImageInfo {},
            .pBufferInfo { &binding1 },
            .pTexelBufferView {}
        };

        VkWriteDescriptorSet write2 {
            .sType { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET },
            .pNext { nullptr },
            .dstSet { m_frameData[i].descriptorSets[0] },
            .dstBinding { 2 },
            .dstArrayElement {},
            .descriptorCount { 1 },
            .descriptorType { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER },
            .pImageInfo { &binding2 },
            .pBufferInfo {},
            .pTexelBufferView {}
        };

        std::vector<VkWriteDescriptorSet> writes { write0, write1, write2 };

        vkUpdateDescriptorSets(DEVICE->GetHandle(), static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
    }
}

void App::CreateTexture(void)
{
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load("C:/assets/images/crate2_diffuse.png", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

    if (!pixels) {
        throw std::runtime_error("failed to load texture image!");
    }

    VkDeviceSize imageSize = texWidth * texHeight * 4;
    VkBufferUsageFlags stagingBufferUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    VmaAllocationCreateFlags stagingBufferAllocationFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

    VK::Buffer stagingBuffer = DEVICE->CreateBuffer(imageSize, stagingBufferUsage, stagingBufferAllocationFlags);
    DEVICE->CopyDataToDevice(stagingBuffer.allocation, pixels, imageSize);

    stbi_image_free(pixels);

    VkExtent3D extent {
        .width { static_cast<uint32_t>(texWidth) },
        .height { static_cast<uint32_t>(texHeight) },
        .depth { 1 }
    };

    texture = DEVICE->CreateImage(extent);
    TransitionImageLayout(texture.handle, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    DEVICE->CopyBufferToImage(stagingBuffer.handle, texture.handle, extent.width, extent.height);
    TransitionImageLayout(texture.handle, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    DEVICE->DestroyBuffer(stagingBuffer);
}

void App::CreateSampler()
{
    VkPhysicalDeviceProperties properties {};
    vkGetPhysicalDeviceProperties(DEVICE->GetPhysicalDeviceHandle(), &properties);

    VkSamplerCreateInfo samplerInfo {
        .sType { VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO },
        .pNext { nullptr },
        .flags {},
        .magFilter { VK_FILTER_LINEAR },
        .minFilter { VK_FILTER_LINEAR },
        .mipmapMode { VK_SAMPLER_MIPMAP_MODE_LINEAR },
        .addressModeU { VK_SAMPLER_ADDRESS_MODE_REPEAT },
        .addressModeV { VK_SAMPLER_ADDRESS_MODE_REPEAT },
        .addressModeW { VK_SAMPLER_ADDRESS_MODE_REPEAT },
        .mipLodBias {},
        .anisotropyEnable { VK_TRUE },
        .maxAnisotropy { properties.limits.maxSamplerAnisotropy },
        .compareEnable { VK_FALSE },
        .compareOp { VK_COMPARE_OP_ALWAYS },
        .minLod {},
        .maxLod {},
        .borderColor { VK_BORDER_COLOR_INT_OPAQUE_BLACK },
        .unnormalizedCoordinates { VK_FALSE }
    };

    CHECK_VK(vkCreateSampler(DEVICE->GetHandle(), &samplerInfo, nullptr, &textureSampler), "Failed to create texture sampler.");
}

void App::TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
    VkCommandBuffer commandBuffer = DEVICE->BeginSingleTimeCommands();

    VkImageSubresourceRange subresourceRange {
        .aspectMask { VK_IMAGE_ASPECT_COLOR_BIT },
        .baseMipLevel { 0 },
        .levelCount { 1 },
        .baseArrayLayer { 0 },
        .layerCount { 1 },
    };

    VkImageMemoryBarrier barrier {
        .sType { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER },
        .pNext { nullptr },
        .srcAccessMask {},
        .dstAccessMask {},
        .oldLayout { oldLayout },
        .newLayout { newLayout },
        .srcQueueFamilyIndex { VK_QUEUE_FAMILY_IGNORED },
        .dstQueueFamilyIndex { VK_QUEUE_FAMILY_IGNORED },
        .image { image },
        .subresourceRange { subresourceRange }
    };

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        throw std::invalid_argument("unsupported layout transition!");
    }

    vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    DEVICE->EndSingleTimeCommands(commandBuffer);
}

void App::Update(void)
{
    GlobalUniformData ubo {
        .view { glm::mat4(1.0f) },
        .proj { glm::mat4(1.0f) }
    };

    DEVICE->CopyDataToDevice(CURRENT_FRAME.globalUBO.allocation, &ubo, sizeof(GlobalUniformData));
}

void App::Render(void)
{
    vkWaitForFences(DEVICE->GetHandle(), 1, &CURRENT_FRAME.inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(DEVICE->GetHandle(), 1, &CURRENT_FRAME.inFlightFence);
    uint32_t imageIndex;
    vkAcquireNextImageKHR(DEVICE->GetHandle(), SWAPCHAIN->GetHandle(), UINT64_MAX, CURRENT_FRAME.imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

    vkResetCommandBuffer(CURRENT_FRAME.commandBuffer, 0);
    recordCommandBuffer(CURRENT_FRAME.commandBuffer, imageIndex);

    VkSemaphore waitSemaphores[] = { CURRENT_FRAME.imageAvailableSemaphore };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    VkSemaphore signalSemaphores[] = { CURRENT_FRAME.renderFinishedSemaphore };

    VkSubmitInfo submitInfo {
        .sType { VK_STRUCTURE_TYPE_SUBMIT_INFO },
        .pNext { nullptr },
        .waitSemaphoreCount { 1 },
        .pWaitSemaphores { waitSemaphores },
        .pWaitDstStageMask { waitStages },
        .commandBufferCount { 1 },
        .pCommandBuffers { &CURRENT_FRAME.commandBuffer },
        .signalSemaphoreCount { 1 },
        .pSignalSemaphores { signalSemaphores }
    };

    CHECK_VK(vkQueueSubmit(DEVICE->GetGrahpicsQueue(), 1, &submitInfo, CURRENT_FRAME.inFlightFence), "Failed to submit command buffer.");

    VkSwapchainKHR swapChains[] = { SWAPCHAIN->GetHandle() };

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

    CHECK_VK(vkQueuePresentKHR(DEVICE->GetPresentQueue(), &presentInfo), "Failed to present image.");

    m_currentFrame = (m_currentFrame + 1) % MAX_FRAME;
}
