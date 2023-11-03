#ifndef LAB_2_GAME_OF_LIFE_APPLICATION_H
#define LAB_2_GAME_OF_LIFE_APPLICATION_H

#include <memory>

#include "core/Universe.h"
#include "frontend/UIRenderer.h"
#include "utils/CommandLineArguments.h"

class Application : public UIRenderer {
public:
    int launch(const CommandLineArguments &cmdArgs);

    void onFrameUpdate() override;
    static void drawField(Field &currField);

    ~Application() override = default;

private:
    std::unique_ptr<Universe> current_universe_;
};

#endif //LAB_2_GAME_OF_LIFE_APPLICATION_H
