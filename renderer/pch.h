#pragma once

// C++
#include <cstdint>
#include <stdexcept>
#include <vector>
#include <memory>
#include <array>
#include <iostream>
#include <cassert>
#include <optional>
#include <filesystem>

// library
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED
#define GLM_FORCE_QUAT_DATA_XYZW
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

// core
#include <vk_core.h>
#include <graphics.h>

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 uv;
};

struct MeshUniformData {
    glm::mat4 world;
    VK::Image diffuseImage;
    VK::ImageView diffuseImageView;
};

struct GlobalUniformData {
    glm::mat4 view;
    glm::mat4 proj;
};

struct FrameData {
    VK::CommandBuffer commandBuffer;
    VK::Semaphore imageAvailableSemaphore;
    VK::Semaphore renderFinishedSemaphore;
    VK::Fence inFlightFence;
    VK::Buffer globalUBO;
};

struct GuiOptions {
    bool wireframe;
};