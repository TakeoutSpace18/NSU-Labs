package nsu.urdin.tetris.view;

import javax.swing.*;
import java.awt.*;

public class BlockPanel extends JPanel {
    public static final int SIZE = 100;
    private final Color color;
    private double velocityX;
    private double velocityY;
    private double posX;
    private double posY;

    public BlockPanel(Color color) {
        this.color = color;
        this.posX = 10;
        this.posY = 10;
        this.velocityY = 2;
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
        posX += velocityX;
        posY += velocityY;
        setLocation((int) (posX), (int) (posY));
    }
}
