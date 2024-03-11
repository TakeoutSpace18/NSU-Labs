package nsu.urdin.tetris.view;

import nsu.urdin.tetris.utils.Vec2d;

import javax.swing.*;
import javax.swing.text.Position;
import java.awt.*;

public class BlockPanel extends JPanel {
    public static final int SIZE = 100;
    private final Color color;
    private Vec2d velocity;
    private Vec2d position;

    public BlockPanel(Color color) {
        this.color = color;
        this.position = Vec2d.of(10, 10);
        this.velocity = Vec2d.of(10, 5);
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        g.setColor(color);
        g.fillRect(0, 0, SIZE, SIZE);

        // this fixes animation freezes on linux
        Toolkit.getDefaultToolkit().sync();
    }

    @Override
    public Dimension getPreferredSize() {
        return new Dimension(SIZE, SIZE);
    }

    public void update() {
        position = position.add(velocity);
        setLocation((int) (position.x()), (int) (position.y()));
    }
}
