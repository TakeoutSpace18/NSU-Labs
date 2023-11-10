#ifndef LAB_2_GAME_OF_LIFE_APPLICATION_H
#define LAB_2_GAME_OF_LIFE_APPLICATION_H

#include <memory>
#include <chrono>

#include "core/Universe.h"
#include "frontend/UIRenderer.h"
#include "utils/CommandLineArguments.h"

class Application : public UIRenderer {
public:
    Application();
    int launch(const CommandLineArguments& cmdArgs);

    void onFrameUpdate() override;

    ~Application() override = default;

private:
    void fieldWindowUpdate() const;
    void controlWindowUpdate();

    void updateField(Field &curr_field) const;

    int offlineMode(const CommandLineArguments& cmdArgs);

    static std::chrono::milliseconds speedToDelay(uint32_t speed);

    std::unique_ptr<Universe> current_universe_;
    uint32_t play_speed;
    std::chrono::milliseconds delay_between_ticks_;
    bool is_playing_;
};

#endif //LAB_2_GAME_OF_LIFE_APPLICATION_H
