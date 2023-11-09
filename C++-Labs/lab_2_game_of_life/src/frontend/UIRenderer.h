#ifndef LAB_2_GAME_OF_LIFE_UIRENDERER_H
#define LAB_2_GAME_OF_LIFE_UIRENDERER_H

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#include <memory>

#include "core/Universe.h"

class UIRenderer {
public:

    struct Config {
        int window_width;
        int window_height;
        std::string window_title;
    };

    UIRenderer() = default;
    int run(Config config);

    virtual void onFrameUpdate() = 0;

    virtual ~UIRenderer() = default;

private:
    int initialize();
    int mainLoop();

    Config config_;
    GLFWwindow *window_;
};

#endif //LAB_2_GAME_OF_LIFE_UIRENDERER_H
