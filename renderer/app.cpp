#include "pch.h"
#include "app.h"
#include "utils.h"
#include <stb_image.h>

#define CURRENT_FRAME m_frameData[m_currentFrame]
#define DEVICE p_wsi->GetDevice()
#define SWAPCHAIN p_wsi->GetSwapchain()
#define COMMANDPOOL p_wsi->GetCommandPool()
#define ALLOCATOR p_wsi->GetMemoryAllocator()

App::App(uint32_t width, uint32_t height)
    : m_width { width }
    , m_height { height }
{
    CreateGLFW();
    CreateWSI();
    CreateMesh();
    CreateDepthResources();
    CreateTexture();
    CreateSampler();
    CreateRenderPass();
    CreateDescriptorSetLayout();
    CreatePipeline();
    SWAPCHAIN->CreateFrameBuffers(renderPass, depthImageView.GetHandle());
    CreateCommandPool();
    CreateCommandBuffer();
    CreateSyncObjects();
    CreateUniformBuffer();
    CreateDescriptorPool();
    CreateDescriptorSets();
}

App::~App()
{
    delete m_model;
    delete m_mesh;
    vkDestroySampler(DEVICE->GetHandle(), textureSampler, nullptr);
    textureImageView.Destroy();
    depthImageView.Destroy();
    texture.Destroy();
    depthImage.Destroy();

    for (uint32_t i = 0; i < MAX_FRAME; i++) {
        m_frameData[i].globalUBO.Destroy();
        m_frameData[i].commandBuffer.Destroy();
        m_frameData[i].imageAvailableSemaphore.Destroy();
        m_frameData[i].renderFinishedSemaphore.Destroy();
        m_frameData[i].inFlightFence.Destroy();
    }

    descriptorPool.Destroy();
    commandPool.Destroy();
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

    p_wsi = new VK::WSI {};
    p_wsi->Initialize(instanceLayers, instanceExtensions, deviceExtensions, hinstance, hwnd);
}

void App::CreateRenderPass(void)
{
    VkAttachmentDescription depthAttachment {
        .flags {},
        .format { VK_FORMAT_D24_UNORM_S8_UINT },
        .samples { VK_SAMPLE_COUNT_1_BIT },
        .loadOp { VK_ATTACHMENT_LOAD_OP_CLEAR },
        .storeOp { VK_ATTACHMENT_STORE_OP_DONT_CARE },
        .stencilLoadOp { VK_ATTACHMENT_LOAD_OP_DONT_CARE },
        .stencilStoreOp { VK_ATTACHMENT_STORE_OP_DONT_CARE },
        .initialLayout { VK_IMAGE_LAYOUT_UNDEFINED },
        .finalLayout { VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL },
    };

    VkAttachmentReference depthAttachmentRef {
        .attachment { 1 },
        .layout { VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL }
    };

    VkAttachmentDescription colorAttachment {
        .flags {},
        .format { SWAPCHAIN->GetFormat().format },
        .samples { VK_SAMPLE_COUNT_1_BIT },
        .loadOp { VK_ATTACHMENT_LOAD_OP_CLEAR },
        .storeOp { VK_ATTACHMENT_STORE_OP_STORE },
        .stencilLoadOp {},
        .stencilStoreOp {},
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
        .pDepthStencilAttachment { &depthAttachmentRef },
        .preserveAttachmentCount {},
        .pPreserveAttachments {}
    };

    VkSubpassDependency dependency {
        .srcSubpass { VK_SUBPASS_EXTERNAL },
        .dstSubpass { 0 },
        .srcStageMask { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT },
        .dstStageMask { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT },
        .srcAccessMask { 0 },
        .dstAccessMask { VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT },
        .dependencyFlags {}
    };

    std::vector<VkAttachmentDescription> attachments { colorAttachment, depthAttachment };

    VkRenderPassCreateInfo renderPassCreateInfo {
        .sType { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO },
        .pNext { nullptr },
        .flags {},
        .attachmentCount { static_cast<uint32_t>(attachments.size()) },
        .pAttachments { attachments.data() },
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

    VkDescriptorSetLayoutBinding binding2 { //
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

    VK::Shader vertexShader;
    vertexShader.Initialize(DEVICE->GetHandle(), vertexShaderCode);
    VK::Shader fragmentShader;
    fragmentShader.Initialize(DEVICE->GetHandle(), fragmentShaderCode);

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
        .format { VK_FORMAT_R32G32B32_SFLOAT },
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
        .sType { VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO },
        .pNext { nullptr },
        .flags {},
        .depthTestEnable { VK_TRUE },
        .depthWriteEnable { VK_TRUE },
        .depthCompareOp { VK_COMPARE_OP_LESS },
        .depthBoundsTestEnable { VK_FALSE },
        .stencilTestEnable { VK_FALSE },
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
    commandPool = DEVICE->CreateCommandPool(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
}

void App::CreateCommandBuffer()
{
    for (uint32_t i = 0; i < MAX_FRAME; i++) {
        m_frameData[i].commandBuffer = commandPool.AllocateCommandBuffer(DEVICE->GetGrahpicsQueue());
    }
}

void App::CreateSyncObjects(void)
{
    for (uint32_t i = 0; i < MAX_FRAME; i++) {
        m_frameData[i].imageAvailableSemaphore.Initialize(DEVICE->GetHandle());
        m_frameData[i].renderFinishedSemaphore.Initialize(DEVICE->GetHandle());
        m_frameData[i].inFlightFence.Initialize(DEVICE->GetHandle(), VK_FENCE_CREATE_SIGNALED_BIT);
    }
}

void App::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
    VkRect2D renderArea {
        .offset { 0, 0 },
        .extent { SWAPCHAIN->GetExtent() }
    };

    std::vector<VkClearValue> clearValues { { { 0.0f, 0.0f, 0.0f, 1.0f } }, { 1.0f, 0.0f } };

    VkRenderPassBeginInfo renderPassInfo {
        .sType { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO },
        .pNext { nullptr },
        .renderPass { renderPass },
        .framebuffer { SWAPCHAIN->GetFrameBuffer(imageIndex) },
        .renderArea { renderArea },
        .clearValueCount { static_cast<uint32_t>(clearValues.size()) },
        .pClearValues { clearValues.data() }
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

    m_model->Render(commandBuffer, pipelineLayout);

    vkCmdEndRenderPass(commandBuffer);
}

void App::CreateMesh(void)
{
    m_model = new Model {};
    m_model->ReadFromFile("C:/assets/glTF-Sample-Models/2.0/Box/glTF/Box.gltf", DEVICE, ALLOCATOR, COMMANDPOOL);
}

void App::CreateUniformBuffer(void)
{
    VkDeviceSize bufferSize = sizeof(GlobalUniformData);
    VkBufferUsageFlags bufferUsage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    VmaAllocationCreateFlags allocationFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

    for (uint32_t i = 0; i < MAX_FRAME; i++) {
        m_frameData[i].globalUBO = ALLOCATOR->CreateBuffer(bufferSize, bufferUsage, allocationFlags);
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

    uint32_t maxSets = 1 * MAX_FRAME;

    descriptorPool = DEVICE->CreateDescriptorPool(maxSets, pools);
}

void App::CreateDescriptorSets(void)
{
    for (auto& m : m_model->m_meshes) {
        m->m_descriptorSet = descriptorPool.AllocateDescriptorSet(&descriptorSetLayout);

        m->m_descriptorSet.WriteBuffer(0, { m->GetUniformBuffer(), 0, sizeof(MeshUniformData) });
        m->m_descriptorSet.WriteBuffer(1, { m_frameData[0].globalUBO.GetHandle(), 0, sizeof(GlobalUniformData) });
        m->m_descriptorSet.WriteImage(2, { textureSampler, textureImageView.GetHandle(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL });
    }
}

void App::CreateDepthResources(void)
{
    VkFormat depthFormat = VK_FORMAT_D24_UNORM_S8_UINT;

    VkExtent3D extent3D = { SWAPCHAIN->GetExtent().width, SWAPCHAIN->GetExtent().height, 1 };
    depthImage = ALLOCATOR->CreateImage(extent3D, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);

    depthImageView = depthImage.CreateView(DEVICE->GetHandle(), depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
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

    VK::Buffer stagingBuffer = ALLOCATOR->CreateBuffer(imageSize, stagingBufferUsage, stagingBufferAllocationFlags);
    stagingBuffer.CopyData(pixels, imageSize);

    stbi_image_free(pixels);

    VkExtent3D extent3D = { static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 1 };

    texture = ALLOCATOR->CreateImage(extent3D, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);

    texture.TransitionLayout(COMMANDPOOL->AllocateCommandBuffer(DEVICE->GetGrahpicsQueue()), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    stagingBuffer.CopyToImage(COMMANDPOOL->AllocateCommandBuffer(DEVICE->GetGrahpicsQueue()), texture.GetHandle(), extent3D);
    texture.TransitionLayout(COMMANDPOOL->AllocateCommandBuffer(DEVICE->GetGrahpicsQueue()), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    textureImageView = texture.CreateView(DEVICE->GetHandle(), VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
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

void App::Update(void)
{
    GlobalUniformData ubo {
        .view { glm::mat4(1.0f) },
        .proj { glm::mat4(1.0f) }
    };

    float aspect = static_cast<float>(SWAPCHAIN->GetExtent().width) / SWAPCHAIN->GetExtent().height;
    ubo.view = glm::lookAtLH(glm::vec3(0.0f, 2.0f, -2.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    ubo.proj = glm::perspective(glm::radians(60.0f), aspect, 0.1f, 50.0f);

    CURRENT_FRAME.globalUBO.CopyData(&ubo, sizeof(GlobalUniformData));
}

void App::Render(void)
{
    std::vector<VkFence> fences = { CURRENT_FRAME.inFlightFence.GetHandle() };

    vkWaitForFences(DEVICE->GetHandle(), static_cast<uint32_t>(fences.size()), fences.data(), VK_TRUE, UINT64_MAX);
    vkResetFences(DEVICE->GetHandle(), static_cast<uint32_t>(fences.size()), fences.data());
    uint32_t imageIndex;
    vkAcquireNextImageKHR(DEVICE->GetHandle(), SWAPCHAIN->GetHandle(), UINT64_MAX, CURRENT_FRAME.imageAvailableSemaphore.GetHandle(), VK_NULL_HANDLE, &imageIndex);

    VK::CommandBuffer& commandBuffer = CURRENT_FRAME.commandBuffer;

    commandBuffer.Reset();
    commandBuffer.BeginRecording();
    recordCommandBuffer(commandBuffer.GetHandle(), imageIndex);
    commandBuffer.EndRecording();

    std::vector<VkSemaphore> waitSemaphores = { CURRENT_FRAME.imageAvailableSemaphore.GetHandle() };
    VkPipelineStageFlags waitStages = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    std::vector<VkSemaphore> signalSemaphores = { CURRENT_FRAME.renderFinishedSemaphore.GetHandle() };

    commandBuffer.Submit(waitSemaphores, signalSemaphores, CURRENT_FRAME.inFlightFence.GetHandle());

    VkSwapchainKHR swapChains[] = { SWAPCHAIN->GetHandle() };

    VkPresentInfoKHR presentInfo {
        .sType { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR },
        .pNext { nullptr },
        .waitSemaphoreCount { static_cast<uint32_t>(signalSemaphores.size()) },
        .pWaitSemaphores { signalSemaphores.data() },
        .swapchainCount { 1 },
        .pSwapchains { swapChains },
        .pImageIndices { &imageIndex },
        .pResults { nullptr },
    };

    CHECK_VK(vkQueuePresentKHR(DEVICE->GetPresentQueue(), &presentInfo), "Failed to present image.");

    m_currentFrame = (m_currentFrame + 1) % MAX_FRAME;
}
