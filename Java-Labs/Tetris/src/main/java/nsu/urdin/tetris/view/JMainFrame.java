package nsu.urdin.tetris.view;

import javax.swing.*;
import java.awt.*;

public class JMainFrame extends JFrame {
    private JMainMenuPanel mainMenuPanel;
    private JPanel contentPanel;
    private JGamePanel gamePanel;


    public JMainFrame() throws HeadlessException {
        setContentPane(contentPanel);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        pack();
        setVisible(true);
    }

    public static void main(String[] args) {
        JFrame frame = new JFrame("MainFrameTest");
        frame.setContentPane(new JMainFrame().contentPanel);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.pack();
        frame.setVisible(true);
    }

    public void showGamePanel() {
        CardLayout layout = (CardLayout) contentPanel.getLayout();
        layout.show(getContentPane(), "GameCard");
    }

    public TetrisFieldListener getTetrisFieldListener() {
        return gamePanel.getTetrisFieldPanel();
    }
}
