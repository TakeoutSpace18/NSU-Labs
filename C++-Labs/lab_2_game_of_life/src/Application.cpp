#include "Application.h"

#include <filesystem>
#include <chrono>

#define IMGUI_USER_CONFIG "frontend/imgui_user_config.h"
#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"

#include "serialize/LifeASCIISerializer.h"
#include "core/Universe.h"

int Application::launch(const CommandLineArguments &cmdArgs) {
    auto input_path = cmdArgs.getOption<std::filesystem::path>("input", "i");
    if (input_path) {
        current_universe_ = LifeASCIISerializer::ReadFromFile(*input_path);
    } else {
        logger << Logger::Warning << "No .life input file specified. Loading default universe...\n";
        current_universe_ = std::make_unique<Universe>(std::initializer_list<std::initializer_list<bool>>{
                {1, 0, 0, 0, 0},
                {1, 1, 0, 0, 0},
                {1, 0, 0, 0, 0},
                {1, 0, 0, 1, 0},
                {0, 0, 0, 0, 1}
        });
    }

    if (cmdArgs.getOption("offline")) {
        return offlineMode(cmdArgs);
    }

    return UIRenderer::run({2000, 1300, "Game of life"});
}

void Application::drawField(const Field &currField) {
    constexpr auto dead_cell_color = IM_COL32(38, 209, 0, 255);
    constexpr auto alive_cell_color = IM_COL32(227, 255, 255, 255);
    constexpr auto border_color = IM_COL32(0, 0, 0, 255);

    float cell_size = 50;
    Vec2f startPos = ImGui::GetCursorScreenPos();
    Vec2f currPos = startPos;

    for (size_t cell_x = 0; cell_x < currField.width(); ++cell_x) {
        for (size_t cell_y = 0; cell_y < currField.height(); ++cell_y) {
            ImU32 color = currField[cell_x][cell_y] ? dead_cell_color : alive_cell_color;
            ImGui::GetWindowDrawList()->AddRectFilled(currPos, currPos + Vec2f(cell_size, cell_size), color);
            ImGui::GetWindowDrawList()->AddRect(currPos, currPos + Vec2f(cell_size, cell_size), border_color);
            currPos.y += cell_size;
        }
        currPos.y = startPos.y;
        currPos.x += cell_size;
    }
}

void Application::onFrameUpdate() {
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
    controlWindowUpdate();
    fieldWindowUpdate();
}

void Application::controlWindowUpdate() {
    ImGui::Begin("Control");
    if (ImGui::Button("Tick")) {
        current_universe_->tick();
    }
    ImGui::SameLine();
    if (ImGui::Button("Play")) {
        is_playing_ = !is_playing_;
    }

    static std::string path;
    ImGui::InputTextWithHint("##", "dump path", &path);
    ImGui::SameLine();
    if (ImGui::Button("Dump")) {
        LifeASCIISerializer::WriteToFile(path, *current_universe_);
    }
    ImGui::End();
}

void Application::fieldWindowUpdate() const {
    ImGui::Begin("Field");
    ImGui::Text("%s", current_universe_->name().c_str());

    using namespace std::chrono_literals;

    static auto time_at_last_tick = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    if (is_playing_ && (now - time_at_last_tick) > 50ms) {
        time_at_last_tick = now;
        current_universe_->tick();
    }

    drawField(current_universe_->field());
    ImGui::End();
}

int Application::offlineMode(const CommandLineArguments &cmdArgs) {
    auto iterations = cmdArgs.getOption<int>("iterations");
    if (!iterations) {
        logger << Logger::Error << "You must specify iterations number in offline mode\n";
        return EXIT_FAILURE;
    }

    current_universe_->tick_n(*iterations);

    auto output_path = cmdArgs.getOption("output", "o");
    if (output_path) {
        LifeASCIISerializer::WriteToFile(*output_path, *current_universe_);
    }
    else {
        logger << Logger::Warning << "No output .life file specified. Saving to ./output.life\n";
        LifeASCIISerializer::WriteToFile("./output.life", *current_universe_);
    }
    return EXIT_SUCCESS;
}
