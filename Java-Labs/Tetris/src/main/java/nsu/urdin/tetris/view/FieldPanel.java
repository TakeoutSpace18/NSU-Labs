package nsu.urdin.tetris.view;

import nsu.urdin.tetris.model.FieldModelImpl;
import nsu.urdin.tetris.utils.Vec2i;

import javax.swing.*;
import java.awt.*;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

public class FieldPanel extends JPanel implements FieldView {

    private ArrayList<ArrayList<BlockPanel>> blockPanelsField;
    private boolean isRunning;
    private Thread gameLoop;

    public FieldPanel() {
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
        blockPanelsField.stream()
                .flatMap(List::stream)
                .filter(Objects::nonNull)
                .forEach(BlockPanel::update);
    }

    @Override
    public void removeBlock(Vec2i position) {

    }

    @Override
    public void setDimension(int width, int height) {
        blockPanelsField = new ArrayList<>(width);
        for (int i = 0; i < width; i++) {
            blockPanelsField.add(new ArrayList<>(height));
        }
    }

    @Override
    public void addBlock(Vec2i position, Color color) {

    }

    @Override
    public void moveBlock(Vec2i oldPosition, Vec2i newPosition) {

    }
}
