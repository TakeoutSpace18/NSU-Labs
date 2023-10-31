#ifndef LAB_2_GAME_OF_LIFE_UIRENDERER_H
#define LAB_2_GAME_OF_LIFE_UIRENDERER_H

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <memory>

#include "core/Universe.h"

class UIRenderer {
public:
    UIRenderer();
    int initialize();
    int mainLoop();
    virtual void onFrameUpdate() = 0;

private:
    GLFWwindow *window_;
};

#endif //LAB_2_GAME_OF_LIFE_UIRENDERER_H
