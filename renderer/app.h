#pragma once

class App {
public:
    App(uint32_t width, uint32_t height);
    ~App();

public:
    void Run(void);

private:
    uint32_t m_width;
    uint32_t m_height;
    GLFWwindow* m_window;
};
