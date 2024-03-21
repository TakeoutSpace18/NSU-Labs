package nsu.urdin.tetris.view;

import lombok.Getter;

import javax.swing.*;

public class JGamePanel {
@Getter
    private JTetrisFieldPanel tetrisFieldPanel;
    private JPanel contentPanel;

    private void createUIComponents() {
        tetrisFieldPanel = new JTetrisFieldPanel();
    }

    public static void main(String[] args) {
        JFrame frame = new JFrame("JGamePanel");
        frame.setContentPane(new JGamePanel().contentPanel);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.pack();
        frame.setVisible(true);
    }

}
