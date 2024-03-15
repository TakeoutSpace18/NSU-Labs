package nsu.urdin.tetris.view;

import nsu.urdin.tetris.utils.Vec2i;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.util.Arrays;
import java.util.Objects;

public class JTetrisField extends JPanel implements TetrisFieldListener {

    private JBlock[][] blocks;
    private JBlock[][] blocksStaging;
    private boolean isRunning;
    private Timer gameLoop;

    public JTetrisField(Vec2i matrixDimensions) {
        // disable layout in order to manually control positions of the blocks
        setLayout(null);

        this.isRunning = true;
        setupMatrix(matrixDimensions);
        setupGameLoop();
        gameLoop.start();
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
    }

    @Override
    public Dimension getPreferredSize() {
        return new Dimension(blocks.length * JBlock.SIZE, blocks[0].length * JBlock.SIZE);
    }

    private void setupGameLoop() {
        gameLoop = new Timer(15, (ActionEvent e) -> {
             if (isRunning) {
                 update();
             }
        });
    }

    private void update() {
        Arrays.stream(blocks)
                .flatMap(Arrays::stream)
                .filter(Objects::nonNull)
                .forEach(JBlock::update);
    }

    @Override
    public void removeBlock(Vec2i pos) {
        if (blocks[pos.x()][pos.y()] == null) {
            throw new RuntimeException("Failed to move block: block doesn't exist on this position");
        }
        remove(blocksStaging[pos.x()][pos.y()]);
        blocksStaging[pos.x()][pos.y()] = null;
    }

    public void setupMatrix(Vec2i dimensions) {
        blocks = new JBlock[dimensions.x()][dimensions.y()];
        blocksStaging = new JBlock[dimensions.x()][dimensions.y()];
    }

    @Override
    public void addBlock(Vec2i pos, Color color) {
        if (blocksStaging[pos.x()][pos.y()] != null) {
            throw new RuntimeException("Failed to add block: position already occupied");
        }
        JBlock newBlock = new JBlock(pos, color);
        blocksStaging[pos.x()][pos.y()] = newBlock;
        add(newBlock);
    }

    @Override
    public void moveBlock(Vec2i oldPos, Vec2i newPos) {
        if (blocks[oldPos.x()][oldPos.y()] == null) {
            throw new RuntimeException("Failed to move block: block doesn't exist on this position");
        }

        JBlock block = blocks[oldPos.x()][oldPos.y()];
        block.setTargetPosition(newPos);

        blocksStaging[newPos.x()][newPos.y()] = block;
        if (blocksStaging[oldPos.x()][oldPos.y()] == block) {
            blocksStaging[oldPos.x()][oldPos.y()] = null;
        }
    }

    @Override
    public synchronized void applyChanges() {
        blocks = Arrays.stream(blocksStaging)
                .map(JBlock[]::clone)
                .toArray(JBlock[][]::new);
        updateUI();
    }
}
