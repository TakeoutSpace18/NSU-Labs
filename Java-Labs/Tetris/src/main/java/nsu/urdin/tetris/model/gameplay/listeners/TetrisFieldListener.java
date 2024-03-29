package nsu.urdin.tetris.model.gameplay.listeners;

import nsu.urdin.tetris.utils.Vec2i;

import java.awt.*;

public interface TetrisFieldListener {
    void addBlock(Vec2i position, Color color);
    void removeBlock(Vec2i position);
    void removeLineOfBlocks(int line);
    void moveBlock(Vec2i oldPosition, Vec2i newPosition);
    void moveLineOfBlocks(int line, int offset);
    void clear();
    void applyChanges();
}
