#pragma once

// C++
#include <cstdint>
#include <exception>
#include <vector>

// library
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

// core
#include <vk_core.h>