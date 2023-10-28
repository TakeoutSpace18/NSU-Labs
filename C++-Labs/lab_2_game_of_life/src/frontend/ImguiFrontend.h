#ifndef LAB_2_GAME_OF_LIFE_IMGUIFRONTEND_H
#define LAB_2_GAME_OF_LIFE_IMGUIFRONTEND_H

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <memory>

#include "core/Universe.h"

class ImguiFrontend {
public:
    int initialize();
    int mainLoop(std::shared_ptr<Universe> currUniverse);
    void drawField(Field &currField);

private:
    GLFWwindow *window_;
};


#endif //LAB_2_GAME_OF_LIFE_IMGUIFRONTEND_H
