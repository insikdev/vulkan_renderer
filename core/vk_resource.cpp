#include "pch.h"
#include "vk_resource.h"

VK::Resource::Resource(VmaAllocator allocator)
    : m_allocator { allocator }
{
}

VK::Resource::~Resource() { }