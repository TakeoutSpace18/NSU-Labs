#include "Application.h"

#include <memory>

#include "frontend/ImguiFrontend.h"
#include "core/Universe.h"

int Application::launch(int argc, char **argv) {
    auto universe = std::make_shared<Universe>(std::initializer_list<std::initializer_list<bool>>{
        {1, 0, 0, 0, 0},
        {1, 1, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 1, 0},
        {0, 0, 0, 0, 1}
    });

    std::set<uint8_t> born = {3};
    std::set<uint8_t> survive = {2, 3};
    universe->setRules(std::move(born), std::move(survive));

    auto frontend = std::make_unique<ImguiFrontend>();
    frontend->initialize();
    return frontend->mainLoop(universe);
}
