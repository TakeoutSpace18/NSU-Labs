#ifndef LAB_2_GAME_OF_LIFE_APPLICATION_H
#define LAB_2_GAME_OF_LIFE_APPLICATION_H

#include <memory>
#include <chrono>
#include <filesystem>
#include <bitset>

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
    void debugInfoWindowUpdate() const;

    static void updateField(Field &curr_field) ;

    int offlineMode(const CommandLineArguments& cmdArgs);

    void openUniverse(const std::filesystem::path& path);

    static constexpr std::chrono::milliseconds speedToDelay(uint32_t speed);

    void openUniverseButton();
    void dumpAsButton();
    void rulesSelector();

    std::unique_ptr<Universe> current_universe_;
    uint32_t play_speed;
    std::chrono::milliseconds delay_between_ticks_;
    bool is_playing_;
    std::string dump_path_;
    uint32_t field_size_[2];

};

#endif //LAB_2_GAME_OF_LIFE_APPLICATION_H
