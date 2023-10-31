#ifndef LAB_2_GAME_OF_LIFE_GAMEOFLIFEUI_H
#define LAB_2_GAME_OF_LIFE_GAMEOFLIFEUI_H

#include "core/Field.h"
#include "UIRenderer.h"

class GameOfLifeUI : UIRenderer {

    virtual ~GameOfLifeUI() = default;

    void onFrameUpdate() override;
    void drawField(Field &currField);
};


#endif //LAB_2_GAME_OF_LIFE_GAMEOFLIFEUI_H
