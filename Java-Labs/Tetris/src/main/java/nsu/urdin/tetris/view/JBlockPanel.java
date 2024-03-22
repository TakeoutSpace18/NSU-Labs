package nsu.urdin.tetris.view;

import nsu.urdin.tetris.utils.Vec2d;
import nsu.urdin.tetris.utils.Vec2i;

import javax.swing.*;
import java.awt.*;

public class JBlockPanel extends JPanel {
    public static final int SIZE = 35;
    private final Color color;
    private Vec2d velocity;
    private Vec2d position;
    private Vec2d targetPosition;

    public JBlockPanel(Vec2i matrixPos, Color color) {
        this.color = color;
        this.position = convertToPixelPosition(matrixPos);
        this.targetPosition = position;
        this.velocity = Vec2d.of(0, 0);

        setBounds((int) (position.x()), (int) (position.y()), SIZE, SIZE);
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        g.setColor(color);
        g.fillRect(0, 0, SIZE, SIZE);
        g.setColor(color.darker());
        int offset = (int) (SIZE * 0.13);
        g.fillRect(offset, offset, SIZE - 2 * offset, SIZE - 2 * offset);

        // this fixes animation freezes on linux
        Toolkit.getDefaultToolkit().sync();
    }

    @Override
    public Dimension getPreferredSize() {
        return new Dimension(SIZE, SIZE);
    }

    public void update() {
        position = position.add(velocity);

        if (position.equalsEps(targetPosition, 0.01)) {
            position = targetPosition;
            velocity = Vec2d.of(0, 0);
        }
        setBounds((int) (position.x()), (int) (position.y()), SIZE, SIZE);
    }

    public void setTargetPosition(Vec2i matrixTargetPos) {
        this.targetPosition = convertToPixelPosition(matrixTargetPos);
        this.velocity = targetPosition.subtract(position).scalarMul(0.2);
    }

    private Vec2d convertToPixelPosition(Vec2i matrixPos) {
        return new Vec2d(matrixPos.x() * SIZE, matrixPos.y() * SIZE);
    }
}
