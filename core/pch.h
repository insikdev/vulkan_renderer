#pragma once

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#include <vulkan\vulkan.h>

#include <cstdint>
#include <cassert>
#include <vector>
#include <memory>
#include <set>