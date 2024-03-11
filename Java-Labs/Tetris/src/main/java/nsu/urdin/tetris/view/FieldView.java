package nsu.urdin.tetris.view;

import nsu.urdin.tetris.utils.Vec2i;

import java.awt.*;

public interface FieldView {
    void setDimension(int width, int height);
    void addBlock(Vec2i position, Color color);
    void removeBlock(Vec2i position);
    void moveBlock(Vec2i oldPosition, Vec2i newPosition);
}
