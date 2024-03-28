package nsu.urdin.tetris.model.gameplay;

import lombok.Builder;
import lombok.Getter;
import lombok.Singular;
import nsu.urdin.tetris.utils.Vec2i;
import nsu.urdin.tetris.model.gameplay.listeners.TetrisFieldListener;

import java.awt.*;
import java.util.List;

public class TetrisFigure {
    private final List<TetrisFieldListener> listeners;
    @Getter
    private final List<int[][]> rotations;
    @Getter
    private int curRotation;
    @Getter
    private Vec2i position;
    private final Color color;

    @Builder
    public TetrisFigure(List<TetrisFieldListener> listeners, @Singular List<int[][]> rotations, Color color) {
        this.listeners = listeners;
        this.rotations = rotations;
        this.color = color;
    }


    public void spawn(Vec2i pos, int rotation) {
        curRotation = rotation % rotations.size();
        int[][] blocks = rotations.get(curRotation);
        this.position = pos;

        // notify listeners that blocks have been added
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
        int[][] blocks = rotations.get(curRotation);

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

    public void rotate() {
        int newRotation = (curRotation + 1) % rotations.size();
        int[][] oldBlocks = rotations.get(curRotation);
        int[][] newBlocks = rotations.get(newRotation);

        // notify listeners to remove old blocks
        for (int i = 0; i < oldBlocks.length; ++i) {
            for (int j = 0; j < oldBlocks[0].length; ++j) {
                if (oldBlocks[i][j] != 0) {
                    for (TetrisFieldListener lis : listeners) {
                        lis.removeBlock(position.add(i, j));
                    }
                }
            }
        }
        // notify listeners to add new blocks
        for (int i = 0; i < newBlocks.length; ++i) {
            for (int j = 0; j < newBlocks[0].length; ++j) {
                if (newBlocks[i][j] != 0) {
                    for (TetrisFieldListener lis : listeners) {
                        lis.addBlock(position.add(i, j), color);
                    }
                }
            }
        }

        curRotation = newRotation;
    }

    public int[][] getBlocks() {
        return rotations.get(curRotation);
    }
    public int[][] getRotation(int rotation) {
        return rotations.get(rotation % rotations.size());
    }
    public int[][] getNextRotation() {
        return rotations.get((curRotation + 1) % rotations.size());
    }
}
