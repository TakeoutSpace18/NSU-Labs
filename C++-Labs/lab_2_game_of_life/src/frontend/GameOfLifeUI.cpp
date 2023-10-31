#include "GameOfLifeUI.h"
#include "imgui.h"

void GameOfLifeUI::onFrameUpdate() {

    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
    ImGui::ShowDemoWindow();

    {
        ImGui::Begin("Field");
        if (ImGui::Button("Tick")) {
            currUniverse->tick();
        }
        drawField(currUniverse->field());
        ImGui::End();
    }
}

void GameOfLifeUI::drawField(Field &currField) {
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
