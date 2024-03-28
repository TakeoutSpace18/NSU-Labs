package nsu.urdin.tetris.view;

import lombok.Getter;
import nsu.urdin.tetris.controller.TetrisController;

import javax.swing.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;

public class JPausePanel {
    private JButton resumeButton;
    @Getter
    private JPanel contentPanel;
    private JButton backToMenuButton;

    public JPausePanel() {

        Action resumeAction = new AbstractAction() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                TetrisController.getInstance().resume();
            }
        };

        InputMap inputMap = resumeButton.getInputMap(JComponent.WHEN_IN_FOCUSED_WINDOW);
        inputMap.put(KeyStroke.getKeyStroke(KeyEvent.VK_ESCAPE, 0), "escape");
        resumeButton.addActionListener(resumeAction);
        resumeButton.getActionMap().put("escape", resumeAction);

        backToMenuButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                TetrisController.getInstance().stopGameAndReturnToMainMenu();
            }
        });
    }

}
