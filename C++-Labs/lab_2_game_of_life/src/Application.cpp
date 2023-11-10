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

void Application::updateField(Field &curr_field) const {
    constexpr auto dead_cell_color = IM_COL32(38, 209, 0, 255);
    constexpr auto alive_cell_color = IM_COL32(227, 255, 255, 255);
    constexpr auto border_color = IM_COL32(0, 0, 0, 255);

    float cell_size = (ImGui::GetWindowWidth() - 2 * ImGui::GetStyle().WindowPadding.x) / static_cast<float>(curr_field.width());
    Vec2f startPos = ImGui::GetCursorScreenPos();
    Vec2f currPos = startPos;

    if (ImGui::IsMouseHoveringRect(startPos, startPos + Vec2f{cell_size * curr_field.width(), cell_size * curr_field.height()})) {
        if (ImGui::IsMouseClicked(0)) {
            Vec2f mouse_pos = ImGui::GetMousePos();
            mouse_pos -= startPos;
            size_t cell_x = mouse_pos.x / cell_size;
            size_t cell_y = mouse_pos.y / cell_size;
            curr_field[cell_x][cell_y] = !curr_field[cell_x][cell_y];
        }
    }

    for (size_t cell_x = 0; cell_x < curr_field.width(); ++cell_x) {
        for (size_t cell_y = 0; cell_y < curr_field.height(); ++cell_y) {
            ImU32 color = curr_field[cell_x][cell_y] ? dead_cell_color : alive_cell_color;
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

    static int field_size[2] = {
            static_cast<int>(current_universe_->field().width()),
            static_cast<int>(current_universe_->field().height())
    };
    if (ImGui::SliderInt2("Field size", field_size, 3, 200)) {
        current_universe_->resize(field_size[0], field_size[1]);
    }

    if (ImGui::SliderInt("Speed", reinterpret_cast<int*>(&play_speed), 1, 100)) {
        delay_between_ticks_ = std::chrono::milliseconds(speedToDelay(play_speed));
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
    if (is_playing_ && (now - time_at_last_tick) > delay_between_ticks_) {
        time_at_last_tick = now;
        current_universe_->tick();
    }

    updateField(current_universe_->field());
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

Application::Application()
: play_speed(1), delay_between_ticks_(speedToDelay(play_speed)), is_playing_(false) { }

std::chrono::milliseconds Application::speedToDelay(uint32_t speed) {
    return std::chrono::milliseconds(1000 / speed);
}
