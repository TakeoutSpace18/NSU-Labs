package nsu.urdin.tetris.model;

public class GameFieldModel {
    public static final int WIDTH = 10;
    public static final int HEIGHT = 16;

    private int[][] landedBlocks;
    private Figure fallingFigure;

    public GameFieldModel() {
        landedBlocks = new int[WIDTH][HEIGHT];
        spawnNewFigure();
    }

    void moveFallingFigureByOneStep() {
        if (!checkCollision(fallingFigure.getBlocks(), fallingFigure.getPosX(), fallingFigure.getPosY() + 1)) {
            fallingFigure.setPosY(fallingFigure.getPosY() + 1);
        } else {
            addToLandedBlocks(fallingFigure);
            spawnNewFigure();
        }
    }

    private void spawnNewFigure() {
        fallingFigure = new Figure();
    }

    private void addToLandedBlocks(Figure fallingFigure) {
        int[][] blocks = fallingFigure.getBlocks();

        for (int i = 0; i < blocks.length; ++i) {
            for (int j = 0; j < blocks[0].length; ++j) {
                if (blocks[i][j] != 0) {
                    landedBlocks[fallingFigure.getPosX() + i][fallingFigure.getPosY() + j] = blocks[i][j];
                }
            }
        }
    }

    private boolean checkCollision(int[][] blocks, int newX, int newY) {
        for (int i = 0; i < blocks.length; ++i) {
            for (int j = 0; j < blocks[0].length; ++j) {
                if (blocks[i][j] != 0) {
                    if (!checkPositionWithinField(newX + i, newY + j) || landedBlocks[newX + i][newY + j] != 0) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    private boolean checkPositionWithinField(int posX, int posY) {
        if (posX < 0 || posX >= WIDTH || posY < 0 || posY >= HEIGHT) {
            return false;
        }

        return true;
    }
}
