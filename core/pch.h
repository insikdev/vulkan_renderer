#pragma once

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#include <vulkan\vulkan.h>

#include <stdexcept>
#include <iostream>
#include <vector>
#include <memory>
#include <set>
#include <limits>
#include <cstdint>

#include "check_vk.h"
