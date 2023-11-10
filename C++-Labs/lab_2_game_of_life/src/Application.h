#ifndef LAB_2_GAME_OF_LIFE_APPLICATION_H
#define LAB_2_GAME_OF_LIFE_APPLICATION_H

#include <memory>

#include "core/Universe.h"
#include "frontend/UIRenderer.h"
#include "utils/CommandLineArguments.h"

class Application : public UIRenderer {
public:
    int launch(const CommandLineArguments& cmdArgs);

    void onFrameUpdate() override;
    static void drawField(const Field &currField);

    ~Application() override = default;

private:
    void fieldWindowUpdate() const;
    void controlWindowUpdate();

    int offlineMode(const CommandLineArguments& cmdArgs);

    std::unique_ptr<Universe> current_universe_;
    bool is_playing_;
};

#endif //LAB_2_GAME_OF_LIFE_APPLICATION_H
