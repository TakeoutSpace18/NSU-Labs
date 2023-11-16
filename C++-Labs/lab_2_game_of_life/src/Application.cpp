#include "Application.h"

#include <filesystem>
#include <bitset>
#include <chrono>
#include <nfd.h>

#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"

#include "serialize/LifeASCIISerializer.h"
#include "core/Universe.h"
#include "utils/Vec2.hpp"

int Application::launch(const CommandLineArguments &cmdArgs) {
    auto input_path = cmdArgs.getOption<std::filesystem::path>("input", "i");
    if (input_path) {
        openUniverse(*input_path);
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
    constexpr auto alive_cell_color = IM_COL32(32, 168, 2, 255);
    constexpr auto dead_cell_color = IM_COL32(227, 255, 255, 255);
    constexpr auto border_color = IM_COL32(0, 0, 0, 255);

    float cell_size = (ImGui::GetWindowWidth() - 2 * ImGui::GetStyle().WindowPadding.x) / static_cast<float>(curr_field.width());
    Vec2f startPos = ImGui::GetCursorScreenPos();
    Vec2f currPos = startPos;

    if (ImGui::IsMouseHoveringRect(startPos, startPos + Vec2f{cell_size * curr_field.width(), cell_size * curr_field.height()})) {
        if (ImGui::IsMouseClicked(0)) {
            auto mouse_pos = static_cast<Vec2f>(ImGui::GetMousePos()) - startPos;
            auto cell_x = static_cast<size_t>(mouse_pos.x / cell_size);
            auto cell_y = static_cast<size_t>(mouse_pos.y / cell_size);
            curr_field[cell_x][cell_y] = !curr_field[cell_x][cell_y];
        }
    }

    for (size_t cell_x = 0; cell_x < curr_field.width(); ++cell_x) {
        for (size_t cell_y = 0; cell_y < curr_field.height(); ++cell_y) {
            ImU32 color = curr_field[cell_x][cell_y] ? alive_cell_color : dead_cell_color;
            ImGui::GetWindowDrawList()->AddRectFilled(currPos, currPos + Vec2f(cell_size, cell_size), color);
            if (show_grid_) {
                ImGui::GetWindowDrawList()->AddRect(currPos, currPos + Vec2f(cell_size, cell_size), border_color);
            }
            currPos.y += cell_size;
        }
        currPos.y = startPos.y;
        currPos.x += cell_size;
    }

}

void Application::onFrameUpdate() {
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
    checkKeybindings();
    controlWindowUpdate();
    debugInfoWindowUpdate();
    fieldWindowUpdate();
}

void Application::checkKeybindings() {
    if (ImGui::IsKeyPressed(ImGuiKey_Space)) {
        is_playing_ = !is_playing_;
    }
    if (ImGui::IsKeyPressed(ImGuiKey_T) || ImGui::IsKeyPressed(ImGuiKey_Enter)) {
        current_universe_->tick();
    }
}

void Application::debugInfoWindowUpdate() {
    ImGui::Begin("Debug info");
    auto io = ImGui::GetIO();
    ImGui::Text("Dear ImGui %s", ImGui::GetVersion());
    ImGui::Text("Application average ms/frame: %6.3f", 1000.0f / io.Framerate);
    ImGui::Text("FPS: %.1f ", io.Framerate);
    ImGui::Text("%d vertices, %d indices (%d triangles)", io.MetricsRenderVertices, io.MetricsRenderIndices, io.MetricsRenderIndices / 3);
    ImGui::End();
}

void Application::controlWindowUpdate() {
    ImGui::Begin("Control");

    openUniverseButton();

    if (ImGui::Button("Tick")) {
        current_universe_->tick();
    }
    ImGui::SameLine();
    if (ImGui::Button(is_playing_ ?  "Pause" : "Play")) {
        is_playing_ = !is_playing_;
    }

    ImGui::Separator();
    if (ImGui::SliderInt2("Field size", reinterpret_cast<int *>(field_size_), 3, 700)) {
        if (field_size_[0] < 3) {
            field_size_[0] = 3;
        }
        if (field_size_[1] < 3) {
            field_size_[1] = 3;
        }
        current_universe_->resize(field_size_[0], field_size_[1]);
    }

    if (ImGui::SliderInt("Speed", reinterpret_cast<int*>(&play_speed), 1, 100)) {
        delay_between_ticks_ = std::chrono::milliseconds(speedToDelay(play_speed));
    }

    ImGui::Separator();
    dumpAsButton();

    ImGui::InputTextWithHint("##", "dump path", &dump_path_);
    ImGui::SameLine();
    if (ImGui::Button("Dump")) {
        LifeASCIISerializer::WriteToFile(dump_path_, *current_universe_);
    }

    ImGui::Separator();
    rulesSelector();
    interestingRulesCombo();

    ImGui::Separator();
    ImGui::Checkbox("Show grid", &show_grid_);
    ImGui::End();
}

void Application::interestingRulesCombo() {
    static const char* combo_preview_value = "Choose rule";
    if (ImGui::BeginCombo("Interesting rules", combo_preview_value)) {
        for (auto& [name, rule] : Rules::InterestingRules) {
            if (ImGui::Selectable(name, false)) {
                current_universe_->setRules(rule);
                combo_preview_value = name;
            }

        }
        ImGui::EndCombo();
    }
}

void Application::dumpAsButton() {
    if (ImGui::Button("Dump as")) {
        nfdchar_t* outPath = nullptr;
        nfdresult_t result = NFD_SaveDialog(nullptr, nullptr, &outPath);
        if (result == NFD_OKAY) {
            dump_path_ = std::string(outPath);
            LifeASCIISerializer::WriteToFile(dump_path_, *current_universe_);
            free(outPath);
        }
        else if (result == NFD_ERROR) {
            logger << Logger::Error << NFD_GetError() << "\n";
        }
    }
}

void Application::openUniverseButton() {
    if (ImGui::Button("Open")) {
        nfdchar_t* path = nullptr;
        nfdresult_t result = NFD_OpenDialog(nullptr, nullptr, &path);
        if (result == NFD_OKAY) {
            openUniverse(path);
            free(path);
        }
        else if (result == NFD_ERROR) {
            logger << Logger::Error << NFD_GetError() << "\n";
        }
    }
}

void Application::fieldWindowUpdate() {
    ImGui::Begin("Field");

    std::string name = current_universe_->name();
    if (ImGui::InputTextWithHint("##", "Universe name", &name)) {
        if (name.empty()) {
            name = "Unnamed";
        }
        current_universe_->setName(name);
    };

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

constexpr std::chrono::milliseconds Application::speedToDelay(uint32_t speed) {
    return std::chrono::milliseconds(1000 / speed);
}

void Application::openUniverse(const std::filesystem::path &path) {
    current_universe_ = LifeASCIISerializer::ReadFromFile(path);
    field_size_[0] = current_universe_->field().width();
    field_size_[1] = current_universe_->field().height();
}

void Application::rulesSelector() {
    auto selectorCellSize = Vec2f(40, 40);
    auto born_flags = current_universe_->rules().bornFlags();
    auto survive_flags = current_universe_->rules().surviveFlags();

    ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
    ImGui::Text("Born:");
    ImGui::SameLine(120);
    for (int i = 0; i <= 8; i++) {
        if (i > 0) ImGui::SameLine();
        if (ImGui::Selectable((std::to_string(i) + "##born").c_str(), born_flags[i], 0, selectorCellSize)) {
                born_flags[i].flip();
                current_universe_->setRules(Rules{born_flags, survive_flags});
        }
    }
    ImGui::Text("Survive:");
    ImGui::SameLine(120);
    for (int i = 0; i <= 8; i++) {
        if (i > 0) ImGui::SameLine();
        if (ImGui::Selectable((std::to_string(i) + "##survive").c_str(), survive_flags[i], 0, selectorCellSize)) {
            survive_flags[i].flip();
            current_universe_->setRules(Rules{born_flags, survive_flags});
        }
    }
    ImGui::PopStyleVar();
}

Application::Application() : delay_between_ticks_(speedToDelay(play_speed)) {}
