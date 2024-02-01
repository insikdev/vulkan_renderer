#include "pch.h"
#include "app.h"

App::App(uint32_t width, uint32_t height)
    : m_width { width }
    , m_height { height }
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

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    p_instance = new VK::Instance { {}, extensions };
    p_device = new VK::Device { p_instance, { VK_KHR_SWAPCHAIN_EXTENSION_NAME } };
    p_surface = new VK::WinSurface { p_instance, GetModuleHandle(nullptr), glfwGetWin32Window(m_window) };
    p_swapChain = new VK::Swapchain { p_device, p_surface->GetHandle() };
}

App::~App()
{
    delete p_swapChain;
    delete p_surface;
    delete p_device;
    delete p_instance;
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void App::Run(void)
{
    while (glfwWindowShouldClose(m_window) != GL_TRUE) {
        glfwPollEvents();
    }
}