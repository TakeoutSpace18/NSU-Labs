package nsu.urdin.tetris.model;

import nsu.urdin.tetris.model.listeners.TetrisFieldListener;

import java.awt.*;
import java.util.ArrayList;
import java.util.List;
import java.util.Random;

public class FigureFactory {
    List<TetrisFigure> figures;

    public FigureFactory(List<TetrisFieldListener> listeners) {
        figures = new ArrayList<>();

        TetrisFigure figO = TetrisFigure.builder()
                .rotation(new int[][]
                        {{1, 1},
                         {1, 1}})
                .listeners(listeners)
                .color(new Color(240, 240, 0))
                .build();

        figures.add(figO);

        TetrisFigure figI = TetrisFigure.builder()
                .rotation(new int[][]
                        {{1, 1, 1, 1}})
                .rotation(new int[][]
                        {{1},
                         {1},
                         {1},
                         {1}})
                .listeners(listeners)
                .color(new Color(0, 240, 240))
                .build();

        figures.add(figI);

        TetrisFigure figJ = TetrisFigure.builder()
                .rotation(new int[][]
                        {{1, 1, 1},
                         {0, 0, 1}})
                .rotation(new int[][]
                        {{0, 1},
                         {0, 1},
                         {1, 1}})
                .rotation(new int[][]
                        {{1, 0, 0},
                         {1, 1, 1}})
                .rotation(new int[][]
                        {{1, 1},
                         {1, 0},
                         {1, 0}})
                .listeners(listeners)
                .color(new Color(0, 0, 240))
                .build();

        figures.add(figJ);

        TetrisFigure figL = TetrisFigure.builder()
                .rotation(new int[][]
                        {{0, 0, 1},
                         {1, 1, 1}})
                .rotation(new int[][]
                        {{1, 0},
                         {1, 0},
                         {1, 1}})
                .rotation(new int[][]
                        {{1, 1, 1},
                         {1, 0, 0}})
                .rotation(new int[][]
                        {{1, 1},
                         {0, 1},
                         {0, 1}})
                .listeners(listeners)
                .color(new Color(240, 160, 0))
                .build();

        figures.add(figL);

        TetrisFigure figS = TetrisFigure.builder()
                .rotation(new int[][]
                        {{0, 1, 1},
                         {1, 1, 0}})
                .rotation(new int[][]
                        {{1, 0},
                         {1, 1},
                         {0, 1}})
                .rotation(new int[][]
                        {{0, 1, 1},
                         {1, 1, 0}})
                .rotation(new int[][]
                        {{1, 0},
                         {1, 1},
                         {0, 1}})
                .listeners(listeners)
                .color(new Color(0, 240, 0))
                .build();

        figures.add(figS);

        TetrisFigure figZ = TetrisFigure.builder()
                .rotation(new int[][]
                        {{1, 1, 0},
                         {0, 1, 1}})
                .rotation(new int[][]
                        {{0, 1},
                         {1, 1},
                         {1, 0}})
                .rotation(new int[][]
                        {{1, 1, 0},
                         {0, 1, 1}})
                .rotation(new int[][]
                        {{0, 1},
                         {1, 1},
                         {1, 0}})
                .listeners(listeners)
                .color(new Color(240, 0, 0))
                .build();

        figures.add(figZ);

        TetrisFigure figT = TetrisFigure.builder()
                .rotation(new int[][]
                        {{0, 1, 0},
                         {1, 1, 1}})
                .rotation(new int[][]
                        {{1, 0},
                         {1, 1},
                         {1, 0}})
                .rotation(new int[][]
                        {{1, 1, 1},
                         {0, 1, 0}})
                .rotation(new int[][]
                        {{0, 1},
                         {1, 1},
                         {0, 1}})
                .listeners(listeners)
                .color(new Color(160, 0, 240))
                .build();

        figures.add(figT);
    }

    public TetrisFigure getRandom() {
        int idx = new Random().nextInt(figures.size());
        return figures.get(idx);
    }
}
