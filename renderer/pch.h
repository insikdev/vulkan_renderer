#pragma once

// C++
#include <cstdint>
#include <stdexcept>
#include <vector>
#include <memory>
#include <fstream>
#include <iostream>

// library
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <glm/glm.hpp>

// core
#include <vk_core.h>

struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;
};

struct MeshUniformData {
    glm::mat4 world;
};

struct GlobalUniformData {
    glm::mat4 view;
    glm::mat4 proj;
};