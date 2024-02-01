#include "pch.h"
#include "app.h"

int main(int agrc, char** argv)
{
    const uint32_t width { 1280 };
    const uint32_t height { 720 };
    App app { width, height };
    app.Run();

    return 0;
}