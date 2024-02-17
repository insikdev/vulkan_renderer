#include "pch.h"
#include "app.h"
#include "utils.h"
#include "model.h"
#include "gui.h"

#define CURRENT_FRAME m_frameData[m_currentFrame]

void App::Init(uint32_t width, uint32_t height)
{
    InitGLFW(width, height);
    InitVulkan();
    CreateSampler();
    InitPipeline();
    InitFrameBuffer();
    InitGui();
    InitModel();
}

void App::Destroy(void)
{
#pragma region GUI
    delete p_gui;
    m_guiDescriptorPool.Destroy();
#pragma endregion

    delete m_model;
    vkDestroySampler(m_device.GetHandle(), textureSampler, nullptr);

#pragma region frame data
    for (uint32_t i = 0; i < MAX_FRAME; i++) {
        m_frameData[i].globalUBO.Destroy();
        m_frameData[i].commandBuffer.Destroy();
        m_frameData[i].imageAvailableSemaphore.Destroy();
        m_frameData[i].renderFinishedSemaphore.Destroy();
        m_frameData[i].inFlightFence.Destroy();
    }
#pragma endregion

#pragma region pipeline
    vkDestroyDescriptorSetLayout(m_device.GetHandle(), descriptorSetLayout, nullptr);
    vkDestroyPipeline(m_device.GetHandle(), graphicsPipeline, nullptr);
    vkDestroyPipeline(m_device.GetHandle(), wireGraphicsPipeline, nullptr);
    vkDestroyPipelineLayout(m_device.GetHandle(), pipelineLayout, nullptr);
    vkDestroyRenderPass(m_device.GetHandle(), renderPass, nullptr);
#pragma endregion

#pragma region frame buffer
    for (size_t i = 0; i < m_frameBuffers.size(); i++) {
        m_frameBuffers[i].Destroy();
        m_colorImages[i].Destroy();
    }

    depthImageView.Destroy();
    depthImage.Destroy();
#pragma endregion

#pragma region vulkan object
    m_descriptorPool.Destroy();
    m_transientCommandPool.Destroy();
    m_resetCommandPool.Destroy();
    m_memoryAllocator.Destroy();
    m_swapchain.Destroy();
    m_device.Destroy();
    m_surface.Destroy();
    m_instance.Destroy();
#pragma endregion

#pragma region GLFW
    glfwDestroyWindow(m_window);
    glfwTerminate();
#pragma endregion
}

void App::Run(void)
{
    while (glfwWindowShouldClose(m_window) != GL_TRUE) {
        glfwPollEvents();
        Update();
        Render();
    }

    m_device.WaitIdle();
}

void App::InitGLFW(uint32_t width, uint32_t height)
{
    if (glfwInit() == GLFW_FALSE) {
        std::exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_window = glfwCreateWindow(width, height, "Vulkan", nullptr, nullptr);

    if (m_window == nullptr) {
        std::exit(EXIT_FAILURE);
    }
}

void App::InitVulkan(void)
{
#pragma region instance
    VkApplicationInfo applicationInfo {
        .sType { VK_STRUCTURE_TYPE_APPLICATION_INFO },
        .pNext { nullptr },
        .pApplicationName {},
        .applicationVersion {},
        .pEngineName {},
        .engineVersion {},
        .apiVersion { VK_API_VERSION_1_0 }
    };

    std::vector<const char*> instanceLayers {};
    std::vector<const char*> instanceExtensions { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME };

    CHECK_VK(m_instance.Init(&applicationInfo, instanceLayers, instanceExtensions), "Failed to create instance.");
#pragma endregion

#pragma region surface
    HINSTANCE hinstance = GetModuleHandle(nullptr);
    HWND hwnd = glfwGetWin32Window(m_window);

    CHECK_VK(m_surface.Init(m_instance.GetHandle(), hinstance, hwnd), "Failed to create win32 surface.");
#pragma endregion

#pragma region physical device
    m_physicalDevice = m_instance.GetPhysicalDevices()[0];

    const auto& queueFamilies = m_physicalDevice.GetQueueFamilyProperties();

    uint32_t graphicsQueueFamilyIndex = UINT32_MAX;

    for (size_t i = 0; i < queueFamilies.size(); i++) {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT && m_physicalDevice.CheckPresentationSupport(static_cast<uint32_t>(i), m_surface.GetHandle())) {
            graphicsQueueFamilyIndex = static_cast<uint32_t>(i);
            break;
        }
    }

    if (graphicsQueueFamilyIndex == UINT32_MAX) {
        std::exit(EXIT_FAILURE);
    }
#pragma endregion

#pragma region device
    std::vector<const char*> deviceExtensions { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    float queuePriority = 1.0f;

    VkPhysicalDeviceFeatures deviceFeatures {
        .fillModeNonSolid { VK_TRUE },
        .samplerAnisotropy { VK_TRUE }
    };

    VkDeviceQueueCreateInfo deviceQueueCreateInfo {
        .sType { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO },
        .pNext { nullptr },
        .flags {},
        .queueFamilyIndex { graphicsQueueFamilyIndex },
        .queueCount { 1 },
        .pQueuePriorities { &queuePriority }
    };

    CHECK_VK(m_device.Init(m_physicalDevice.GetHandle(), { deviceQueueCreateInfo }, deviceExtensions, &deviceFeatures), "Failed to create device.");

    m_graphicsQueue = m_device.GetQueue(graphicsQueueFamilyIndex);
#pragma endregion

#pragma region swap chain
    VkSurfaceCapabilitiesKHR surfaceCapabilities = m_physicalDevice.GetSurfaceCapabilities(m_surface.GetHandle());
    m_swapchainImageFormat = VK_FORMAT_R8G8B8A8_SRGB;
    m_swapchainImageExtent = surfaceCapabilities.currentExtent;
    VkColorSpaceKHR colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;

    uint32_t imageCount = surfaceCapabilities.minImageCount + 1;

    if (surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount) {
        imageCount = surfaceCapabilities.maxImageCount;
    }

    CHECK_VK(m_swapchain.Init(m_device.GetHandle(), m_surface.GetHandle(), imageCount, m_swapchainImageFormat, colorSpace, m_swapchainImageExtent, surfaceCapabilities.currentTransform, presentMode), "Failed to create swap chain.");
#pragma endregion

#pragma region memory allocator
    m_memoryAllocator.Init(m_instance.GetHandle(), m_physicalDevice.GetHandle(), m_device.GetHandle());
#pragma endregion

#pragma region command pool
    m_transientCommandPool.Init(m_device.GetHandle(), VK_COMMAND_POOL_CREATE_TRANSIENT_BIT, m_graphicsQueue.GetFamilyIndex());
    m_resetCommandPool.Init(m_device.GetHandle(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, m_graphicsQueue.GetFamilyIndex());

#pragma endregion

#pragma region frame data
    VkDeviceSize bufferSize = sizeof(GlobalUniformData);
    VkBufferUsageFlags bufferUsage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    VmaAllocationCreateFlags allocationFlags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

    for (uint32_t i = 0; i < MAX_FRAME; i++) {
        m_frameData[i].commandBuffer = m_resetCommandPool.AllocateCommandBuffer();
        m_frameData[i].imageAvailableSemaphore.Init(m_device.GetHandle());
        m_frameData[i].renderFinishedSemaphore.Init(m_device.GetHandle());
        m_frameData[i].inFlightFence.Init(m_device.GetHandle(), VK_FENCE_CREATE_SIGNALED_BIT);
        m_frameData[i].globalUBO = m_memoryAllocator.CreateBuffer(bufferSize, bufferUsage, allocationFlags);
    }
#pragma endregion

#pragma region descriptor pool
    std::vector<VkDescriptorPoolSize> pools {
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 },
    };

    for (auto& p : pools) {
        p.descriptorCount *= MAX_FRAME;
    }

    uint32_t maxSets = 1 * MAX_FRAME;

    m_descriptorPool.Init(m_device.GetHandle(), 0, maxSets, pools);
#pragma endregion
}

void App::InitPipeline(void)
{
#pragma region render pass
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
        .format { m_swapchainImageFormat },
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

    CHECK_VK(vkCreateRenderPass(m_device.GetHandle(), &renderPassCreateInfo, nullptr, &renderPass), "Failed to create render pass.");

#pragma endregion

#pragma region descriptor set layout
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

    CHECK_VK(vkCreateDescriptorSetLayout(m_device.GetHandle(), &layoutInfo, nullptr, &descriptorSetLayout), "Failed to create descriptor set layout.");

#pragma endregion

#pragma region pipeline
    const auto& vertexShaderCode = Utils::ReadFile("./shaders/simple.vert.spv");
    const auto& fragmentShaderCode = Utils::ReadFile("./shaders/simple.frag.spv");

    VK::Shader vertexShader;
    vertexShader.Init(m_device.GetHandle(), vertexShaderCode);
    VK::Shader fragmentShader;
    fragmentShader.Init(m_device.GetHandle(), fragmentShaderCode);

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
        .width { static_cast<float>(m_swapchainImageExtent.width) },
        .height { static_cast<float>(m_swapchainImageExtent.height) },
        .minDepth { 0.0f },
        .maxDepth { 1.0f }
    };

    VkRect2D scissor {
        .offset { 0, 0 },
        .extent { m_swapchainImageExtent }
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
        .cullMode { VK_CULL_MODE_NONE },
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

    CHECK_VK(vkCreatePipelineLayout(m_device.GetHandle(), &pipelineLayoutCreateInfo, nullptr, &pipelineLayout), "Failed to create pipeline layout.");

    VkGraphicsPipelineCreateInfo pipelineInfo {
        .sType { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO },
        .pNext { nullptr },
        .flags { VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT },
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

    CHECK_VK(vkCreateGraphicsPipelines(m_device.GetHandle(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline), "Failed to create pipeline");

    rasterizerStateCreateInfo.polygonMode = VK_POLYGON_MODE_LINE;
    pipelineInfo.flags = VK_PIPELINE_CREATE_DERIVATIVE_BIT;
    pipelineInfo.basePipelineHandle = graphicsPipeline;
    pipelineInfo.basePipelineIndex = -1;
    CHECK_VK(vkCreateGraphicsPipelines(m_device.GetHandle(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &wireGraphicsPipeline), "Failed to create pipeline");

#pragma endregion
}

void App::InitFrameBuffer(void)
{
#pragma region color attachment
    const std::vector<VkImage> images = m_swapchain.GetPresentableImages();
    m_colorImages.resize(images.size());

    for (size_t i = 0; i < images.size(); i++) {
        m_colorImages[i].Init(m_device.GetHandle(), images[i], VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
    }
#pragma endregion

#pragma region detph attachment
    VkFormat depthFormat = VK_FORMAT_D24_UNORM_S8_UINT;
    VkExtent3D extent3D = { m_swapchainImageExtent.width, m_swapchainImageExtent.height, 1 };
    depthImage = m_memoryAllocator.CreateImage(extent3D, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
    depthImageView = depthImage.CreateView(m_device.GetHandle(), depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
#pragma endregion

#pragma region frame buffer
    m_frameBuffers.resize(images.size());

    for (size_t i = 0; i < images.size(); i++) {
        std::vector<VkImageView> attachments { m_colorImages[i].GetHandle(), depthImageView.GetHandle() };
        m_frameBuffers[i].Init(m_device.GetHandle(), renderPass, attachments, m_swapchainImageExtent.width, m_swapchainImageExtent.height);
    }
#pragma endregion
}

void App::InitGui(void)
{
    std::vector<VkDescriptorPoolSize> pool_sizes = {
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 },
    };

    m_guiDescriptorPool.Init(m_device.GetHandle(), VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT, 1, pool_sizes);

    ImGui_ImplVulkan_InitInfo info {
        .Instance { m_instance.GetHandle() },
        .PhysicalDevice { m_physicalDevice.GetHandle() },
        .Device { m_device.GetHandle() },
        .QueueFamily { m_graphicsQueue.GetFamilyIndex() },
        .Queue { m_graphicsQueue.GetHandle() },
        .PipelineCache {},
        .DescriptorPool { m_guiDescriptorPool.GetHandle() },
        .Subpass {},
        .MinImageCount { 2 },
        .ImageCount { 3 },
        .MinAllocationSize { 1024 * 1024 }
    };

    p_gui = new GUI {};
    p_gui->Init(m_window, &info, renderPass, &guiOptions);
}

void App::InitModel(void)
{
#pragma region model
    m_model = new Model {};
    m_model->Init("C:/assets/glTF-Sample-Models/2.0/Avocado/glTF/Avocado.gltf", m_device, m_memoryAllocator, m_transientCommandPool, m_graphicsQueue);
#pragma endregion

#pragma region descriptor set
    for (auto& mesh : m_model->m_meshes) {
        mesh.m_descriptorSet = m_descriptorPool.AllocateDescriptorSet(&descriptorSetLayout);

        mesh.m_descriptorSet.WriteBuffer(0, { mesh.m_uniformBuffer.GetHandle(), 0, sizeof(MeshUniformData) });
        mesh.m_descriptorSet.WriteBuffer(1, { m_frameData[0].globalUBO.GetHandle(), 0, sizeof(GlobalUniformData) });
        mesh.m_descriptorSet.WriteImage(2, { textureSampler, mesh.textureView.GetHandle(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL });
    }
#pragma endregion
}

void App::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
    VkRect2D renderArea {
        .offset { 0, 0 },
        .extent { m_swapchainImageExtent }
    };

    std::vector<VkClearValue> clearValues { { { 0.0f, 0.0f, 0.0f, 1.0f } }, { 1.0f, 0.0f } };

    VkRenderPassBeginInfo renderPassInfo {
        .sType { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO },
        .pNext { nullptr },
        .renderPass { renderPass },
        .framebuffer { m_frameBuffers[imageIndex].GetHandle() },
        .renderArea { renderArea },
        .clearValueCount { static_cast<uint32_t>(clearValues.size()) },
        .pClearValues { clearValues.data() }
    };

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    if (guiOptions.wireframe) {
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, wireGraphicsPipeline);
    } else {
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
    }

    VkViewport viewport {
        .x { 0.0f },
        .y { 0.0f },
        .width { static_cast<float>(m_swapchainImageExtent.width) },
        .height { static_cast<float>(m_swapchainImageExtent.height) },
        .minDepth { 0.0f },
        .maxDepth { 1.0f }
    };

    VkRect2D scissor {
        .offset { 0, 0 },
        .extent { m_swapchainImageExtent }
    };

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    m_model->Render(commandBuffer, pipelineLayout);
    p_gui->Render(commandBuffer);

    vkCmdEndRenderPass(commandBuffer);
}

void App::CreateSampler()
{
    VkPhysicalDeviceProperties properties = m_physicalDevice.GetProperties();

    VkSamplerCreateInfo createInfo {
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

    CHECK_VK(vkCreateSampler(m_device.GetHandle(), &createInfo, nullptr, &textureSampler), "Failed to create texture sampler.");
}

void App::Update(void)
{
    GlobalUniformData ubo {
        .view { glm::mat4(1.0f) },
        .proj { glm::mat4(1.0f) }
    };

    float aspect = static_cast<float>(m_swapchainImageExtent.width) / m_swapchainImageExtent.height;
    ubo.view = glm::lookAtLH(glm::vec3(0.0f, 2.0f, -2.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    ubo.proj = glm::perspective(glm::radians(60.0f), aspect, 0.1f, 50.0f);

    CURRENT_FRAME.globalUBO.CopyData(&ubo, sizeof(GlobalUniformData));
}

void App::Render(void)
{
    std::vector<VkFence> fences = { CURRENT_FRAME.inFlightFence.GetHandle() };

    vkWaitForFences(m_device.GetHandle(), static_cast<uint32_t>(fences.size()), fences.data(), VK_TRUE, UINT64_MAX);
    vkResetFences(m_device.GetHandle(), static_cast<uint32_t>(fences.size()), fences.data());

    uint32_t imageIndex;
    m_swapchain.AcquireNextImageIndex(UINT64_MAX, CURRENT_FRAME.imageAvailableSemaphore.GetHandle(), VK_NULL_HANDLE, &imageIndex);

    VK::CommandBuffer& commandBuffer = CURRENT_FRAME.commandBuffer;

    commandBuffer.Reset();
    commandBuffer.BeginRecording();
    recordCommandBuffer(commandBuffer.GetHandle(), imageIndex);
    commandBuffer.EndRecording();

    std::vector<VkSemaphore> waitSemaphores = { CURRENT_FRAME.imageAvailableSemaphore.GetHandle() };
    VkPipelineStageFlags waitStages = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    std::vector<VkSemaphore> signalSemaphores = { CURRENT_FRAME.renderFinishedSemaphore.GetHandle() };
    std::vector<VkCommandBuffer> commandBuffers = { commandBuffer.GetHandle() };

    VkSubmitInfo submitInfo {
        .sType { VK_STRUCTURE_TYPE_SUBMIT_INFO },
        .pNext { nullptr },
        .waitSemaphoreCount { static_cast<uint32_t>(waitSemaphores.size()) },
        .pWaitSemaphores { waitSemaphores.data() },
        .pWaitDstStageMask { &waitStages },
        .commandBufferCount { static_cast<uint32_t>(commandBuffers.size()) },
        .pCommandBuffers { commandBuffers.data() },
        .signalSemaphoreCount { static_cast<uint32_t>(signalSemaphores.size()) },
        .pSignalSemaphores { signalSemaphores.data() }
    };

    m_graphicsQueue.Submit({ submitInfo }, CURRENT_FRAME.inFlightFence.GetHandle());
    m_graphicsQueue.WaitIdle();

    std::vector<VkSwapchainKHR> swapChains = { m_swapchain.GetHandle() };
    m_graphicsQueue.Present(signalSemaphores, swapChains, &imageIndex);

    m_currentFrame = (m_currentFrame + 1) % MAX_FRAME;
}
