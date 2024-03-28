package nsu.urdin.tetris.view;

import nsu.urdin.tetris.model.gameplay.GameplayModelImpl;
import nsu.urdin.tetris.model.gameplay.listeners.TetrisFieldListener;
import nsu.urdin.tetris.utils.Vec2i;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.util.Arrays;
import java.util.Objects;

public class JTetrisFieldPanel extends JPanel implements TetrisFieldListener {

    private JBlockPanel[][] blocks;
    private JBlockPanel[][] blocksStaging;
    private Timer gameLoop;

    public JTetrisFieldPanel() {
        this(GameplayModelImpl.DIMENSIONS);
    }

    public JTetrisFieldPanel(Vec2i matrixDimensions) {
        // disable layout in order to manually control positions of the blocks
        setLayout(null);

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
        return new Dimension(blocks.length * JBlockPanel.SIZE, blocks[0].length * JBlockPanel.SIZE);
    }

    private void setupGameLoop() {
        gameLoop = new Timer(15, (ActionEvent e) -> {
            update();
        });
    }

    private void update() {
        Arrays.stream(blocks)
                .flatMap(Arrays::stream)
                .filter(Objects::nonNull)
                .forEach(JBlockPanel::update);
    }

    @Override
    public void removeBlock(Vec2i pos) {
        if (blocks[pos.x()][pos.y()] == null) {
            throw new RuntimeException("Failed to remove block: block doesn't exist on this position");
        }
        remove(blocksStaging[pos.x()][pos.y()]);
        blocksStaging[pos.x()][pos.y()] = null;
    }

    public void setupMatrix(Vec2i dimensions) {
        blocks = new JBlockPanel[dimensions.x()][dimensions.y()];
        blocksStaging = new JBlockPanel[dimensions.x()][dimensions.y()];
    }

    @Override
    public void addBlock(Vec2i pos, Color color) {
        if (blocksStaging[pos.x()][pos.y()] != null) {
            throw new RuntimeException("Failed to add block: position already occupied");
        }
        JBlockPanel newBlock = new JBlockPanel(pos, color);
        blocksStaging[pos.x()][pos.y()] = newBlock;
        add(newBlock);
    }

    @Override
    public void moveBlock(Vec2i oldPos, Vec2i newPos) {
        if (blocks[oldPos.x()][oldPos.y()] == null) {
            throw new RuntimeException("Failed to move block: block doesn't exist on this position");
        }

        JBlockPanel block = blocks[oldPos.x()][oldPos.y()];
        block.setTargetPosition(newPos);

        blocksStaging[newPos.x()][newPos.y()] = block;
        if (blocksStaging[oldPos.x()][oldPos.y()] == block && !oldPos.equals(newPos)) {
            blocksStaging[oldPos.x()][oldPos.y()] = null;
        }
    }

    @Override
    public void moveLineOfBlocks(int line, int offset) {
        for (int x = 0; x < blocks.length; ++x) {
            if (blocks[x][line] != null) {
                moveBlock(Vec2i.of(x, line), Vec2i.of(x, line + offset));
            }
        }
    }

    @Override
    public void clear() {
        for (int x = 0; x < blocks.length; ++x) {
            for (int y = 0; y < blocks[0].length; ++y) {
                if (blocks[x][y] != null) {
                    removeBlock(Vec2i.of(x, y));
                }
            }
        }
    }

    @Override
    public void removeLineOfBlocks(int line) {
        for (int x = 0; x < blocks.length; ++x) {
            removeBlock(Vec2i.of(x, line));
        }
    }

    @Override
    public synchronized void applyChanges() {
        blocks = Arrays.stream(blocksStaging)
                .map(JBlockPanel[]::clone)
                .toArray(JBlockPanel[][]::new);
        updateUI();
    }
}
