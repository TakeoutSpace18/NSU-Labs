package nsu.urdin.tetris.view;

import nsu.urdin.tetris.model.listeners.TetrisFieldListener;

import javax.swing.*;
import java.awt.*;

public class JMainFrame extends JFrame {
    private JMainMenuPanel mainMenuPanel;
    private JPanel contentPanel;
    private JGamePanel gamePanel;
    private JGameOverPanel JGameOverPanel;


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

    public void showCard(String name) {
        CardLayout layout = (CardLayout) contentPanel.getLayout();
        layout.show(getContentPane(), name);
    }

    public TetrisFieldListener getTetrisFieldListener() {
        return gamePanel.getTetrisFieldPanel();
    }

}
