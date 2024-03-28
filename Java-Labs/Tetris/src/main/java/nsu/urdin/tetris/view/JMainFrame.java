package nsu.urdin.tetris.view;

import nsu.urdin.tetris.model.gameplay.listeners.TetrisFieldListener;

import javax.swing.*;
import java.awt.*;

public class JMainFrame extends JFrame {
    private JMainMenuPanel mainMenuPanel;
    private JPanel contentPanel;
    private JGamePanel gamePanel;
    private JGameOverPanel gameOverPanel;
    private JHighScoresPanel highScoresPanel;
    private JPausePanel pausePanel;

    public JMainFrame() throws HeadlessException {
        setContentPane(contentPanel);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

        pausePanel = new JPausePanel();
        setGlassPane(pausePanel.getContentPanel());
        getGlassPane().setVisible(false);

        ImageIcon icon = new ImageIcon(getClass().getResource("/tetris-icon.png"));
        setIconImage(icon.getImage());

        pack();
        setVisible(true);
    }

    public static void main(String[] args) {
        JFrame frame = new JFrame("MainFrameTest");
        frame.setContentPane(new JMainFrame().contentPanel);
        frame.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
        frame.pack();
        frame.setVisible(true);
    }

    public void showCard(String name) {
        CardLayout layout = (CardLayout) contentPanel.getLayout();
        layout.show(getContentPane(), name);
    }

    public void showPauseMenu() {
        ViewUtils.SetEnabledRecursively(contentPanel, false);
        getGlassPane().setVisible(true);
    }
    public void hidePauseMenu() {
        ViewUtils.SetEnabledRecursively(contentPanel, true);
        getGlassPane().setVisible(false);
    }

    public TetrisFieldListener getTetrisFieldListener() {
        return gamePanel.getTetrisFieldPanel();
    }

}
