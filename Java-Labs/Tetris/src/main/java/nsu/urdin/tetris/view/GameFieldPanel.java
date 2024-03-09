package nsu.urdin.tetris.view;

import nsu.urdin.tetris.model.GameFieldModel;

import javax.swing.*;
import java.awt.*;
import java.util.ArrayList;
import java.util.List;

public class GameFieldPanel extends JPanel {

    private final GameFieldModel gameFieldModel;
    private final List<BlockPanel> blockPanels;
    private boolean isRunning;
    private Thread gameLoop;

    public GameFieldPanel(GameFieldModel gameFieldModel) {
        this.gameFieldModel = gameFieldModel;

        this.blockPanels = new ArrayList<>();
        this.blockPanels.add(new BlockPanel(Color.RED));
        this.blockPanels.forEach(this::add);

        this.isRunning = true;
        setupGameLoop();
        gameLoop.start();
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
    }

    @Override
    public Dimension getPreferredSize() {
        return super.getPreferredSize();
    }

    private void setupGameLoop() {
        gameLoop = new Thread(() -> {
            while (isRunning) {
                this.update();
                try {
                    Thread.sleep(15);
                } catch (InterruptedException e) {
                    throw new RuntimeException(e);
                }
            }
        });
    }

    private void update() {
        blockPanels.forEach(BlockPanel::update);
    }
}
