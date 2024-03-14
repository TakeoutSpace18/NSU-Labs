package nsu.urdin.tetris.model;

import lombok.Getter;
import nsu.urdin.tetris.utils.Vec2i;
import nsu.urdin.tetris.view.TetrisFieldListener;

import java.awt.*;
import java.util.List;

public class TetrisFigure {
    private final List<TetrisFieldListener> listeners;

    @Getter
    private int[][] blocks;

    @Getter
    private Vec2i position;
    private final Color color;

    public TetrisFigure(List<TetrisFieldListener> listeners) {
        this.listeners = listeners;
        blocks = new int[][]{
                {0, 1, 0},
                {1, 1, 1}
        };

        this.color = Color.RED;
        position = Vec2i.of(4, 0);

        for (int i = 0; i < blocks.length; ++i) {
            for (int j = 0; j < blocks[0].length; ++j) {
                if (blocks[i][j] != 0) {
                    for (TetrisFieldListener lis : listeners) {
                        lis.addBlock(position.add(i, j), color);
                    }
                }
            }
        }
    }

    public void move(Vec2i newPosition) {
        // notify listeners that blocks have been moved
        for (int i = 0; i < blocks.length; ++i) {
            for (int j = 0; j < blocks[0].length; ++j) {
                if (blocks[i][j] != 0) {
                    for (TetrisFieldListener lis : listeners) {
                        lis.moveBlock(position.add(i, j), newPosition.add(i, j));
                    }
                }
            }
        }
        position = newPosition;
    }
}
