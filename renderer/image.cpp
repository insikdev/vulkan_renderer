#include "pch.h"
#include "image.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void ImageFile::Init(const std::string& path)
{
    p_data = stbi_load(path.c_str(), &m_width, &m_height, &m_channels, STBI_rgb_alpha);

    if (!p_data) {
        throw std::runtime_error("failed to load texture image!");
    }
}

void ImageFile::Destroy(void)
{
    stbi_image_free(p_data);
}
