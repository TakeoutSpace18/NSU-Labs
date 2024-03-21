package nsu.urdin.tetris.view;

import nsu.urdin.tetris.controller.TetrisController;

import javax.swing.*;
import javax.swing.table.TableColumn;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class JHighScoresPanel {
    private JPanel contentPanel;
    private JTable highScoresTable;
    private JButton backButton;

    public JHighScoresPanel() {
        backButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                TetrisController.getInstance().mainMenu();
            }
        });

    }
}
