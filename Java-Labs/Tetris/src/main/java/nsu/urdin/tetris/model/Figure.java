package nsu.urdin.tetris.model;

import lombok.Getter;
import lombok.Setter;

public class Figure {

    @Getter
    private int[][] blocks;

    @Getter @Setter
    private int posX;

    @Getter @Setter
    private int posY;
    public Figure() {
        blocks = new int[][]{
                {0, 1, 0},
                {1, 1, 1}
        };

        posX = 4;
        posY = 0;
    }
}
