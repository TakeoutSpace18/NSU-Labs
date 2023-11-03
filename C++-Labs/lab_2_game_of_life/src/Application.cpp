#include "Application.h"

#include <memory>

#include "frontend/UIRenderer.h"
#include "core/Universe.h"
#include "imgui.h"

int Application::launch(const CommandLineArguments &cmdArgs) {
    current_universe_ = std::make_unique<Universe>(std::initializer_list<std::initializer_list<bool>>{
        {1, 0, 0, 0, 0},
        {1, 1, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {1, 0, 0, 1, 0},
        {0, 0, 0, 0, 1}
    });

    std::set<uint8_t> born = {3};
    std::set<uint8_t> survive = {2, 3};
    current_universe_->setRules(std::move(born), std::move(survive));

    return mainLoop();
}

void Application::drawField(Field &currField) {
    float cell_size = 50;
    ImVec2 startPos = ImGui::GetCursorScreenPos();
    ImVec2 currPos = startPos;

    for (size_t cell_x = 0; cell_x < currField.width(); ++cell_x) {
        for (size_t cell_y = 0; cell_y < currField.height(); ++cell_y) {
            ImU32 color = currField[cell_x][cell_y] ? IM_COL32(0, 255, 0, 255) : IM_COL32(255, 0, 0, 255);
            ImGui::GetWindowDrawList()->AddRectFilled(currPos, ImVec2(currPos.x + 50, currPos.y + 50), color);
            currPos.x += cell_size;
        }
        currPos.x = startPos.x;
        currPos.y += cell_size;
    }
}

void Application::onFrameUpdate() {
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
    ImGui::ShowDemoWindow();

    {
        ImGui::Begin("Field");
        if (ImGui::Button("Tick")) {
            current_universe_->tick();
        }
        drawField(current_universe_->field());
        ImGui::End();
    }
}
