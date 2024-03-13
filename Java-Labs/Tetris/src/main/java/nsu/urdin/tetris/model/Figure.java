package nsu.urdin.tetris.model;

import lombok.Getter;
import lombok.Setter;
import nsu.urdin.tetris.utils.Vec2i;

public class Figure {

    @Getter
    private int[][] blocks;

    @Getter @Setter
    private Vec2i position;

    public Figure() {
        blocks = new int[][]{
                {0, 1, 0},
                {1, 1, 1}
        };

        position = Vec2i.of(4, 0);
    }
}
