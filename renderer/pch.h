#pragma once

// C++
#include <cstdint>
#include <stdexcept>
#include <vector>
#include <memory>
#include <array>
#include <iostream>

// library
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// core
#include <vk_core.h>

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 uv;
};

struct MeshUniformData {
    glm::mat4 world;
};

struct GlobalUniformData {
    glm::mat4 view;
    glm::mat4 proj;
};

struct FrameData {
    VK::CommandBuffer commandBuffer;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;
    std::vector<VkDescriptorSet> descriptorSets;
    VK::Buffer globalUBO;
};
