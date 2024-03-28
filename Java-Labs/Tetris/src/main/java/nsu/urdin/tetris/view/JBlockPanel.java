package nsu.urdin.tetris.view;

import nsu.urdin.tetris.utils.Vec2d;
import nsu.urdin.tetris.utils.Vec2i;

import javax.swing.*;
import java.awt.*;

public class JBlockPanel extends JPanel {
    public static final int SIZE = 35;
    public static final double SHIFT_VELOCITY_FACTOR = 0.2;
    private final Color color;
    private Color curColor;
    private Vec2d velocity;
    private Vec2d position;
    private Vec2d targetPosition;

    public JBlockPanel(Vec2i matrixPos, Color color) {
        this.color = color;
        this.curColor = color;
        this.position = convertToPixelPosition(matrixPos);
        this.targetPosition = position;
        this.velocity = Vec2d.of(0, 0);

        setBounds((int) (position.x()), (int) (position.y()), SIZE, SIZE);
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        g.setColor(curColor);
        g.fillRect(0, 0, SIZE, SIZE);
        g.setColor(curColor.darker());
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
        this.velocity = targetPosition.subtract(position).scalarMul(SHIFT_VELOCITY_FACTOR);
    }

    private Vec2d convertToPixelPosition(Vec2i matrixPos) {
        return new Vec2d(matrixPos.x() * SIZE, matrixPos.y() * SIZE);
    }

    @Override
    public void setEnabled(boolean enabled) {
        super.setEnabled(enabled);
        if (enabled) {
            curColor = color;
        }
        else {
            curColor = JMainFrame.INACTIVE_COLOR;
        }
    }
}
