package nsu.urdin.tetris.view;

import nsu.urdin.tetris.controller.TetrisController;
import nsu.urdin.tetris.model.Scoreboard;
import nsu.urdin.tetris.model.listeners.ScoreboardListener;

import javax.swing.*;
import javax.swing.table.DefaultTableModel;
import javax.swing.table.TableColumn;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.time.format.DateTimeFormatter;
import java.util.List;

public class JHighScoresPanel {
    private JPanel contentPanel;
    private JTable highScoresTable;
    private JButton backButton;
    private JLabel highScoreLabel;

    public JHighScoresPanel() {
        backButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                TetrisController.getInstance().mainMenu();
            }
        });

        TetrisController.getInstance().getScoreboard().addListener(new ScoreboardListener() {
            @Override
            public void highScoreChanged(Scoreboard.Entry entry) {
                highScoreLabel.setText("High score: " + entry.score() + " (" +
                        entry.date().format(DateTimeFormatter.ISO_LOCAL_DATE_TIME) + ")");
            }

            @Override
            public void scoreTableChanged(List<Scoreboard.Entry> scoreTable) {
                DefaultTableModel model = new DefaultTableModel();
                model.addColumn("Date");
                model.addColumn("Score");
                scoreTable.forEach(entry -> {
                    model.addRow(new Object[]{entry.date().toString(), entry.score()});
                });
                highScoresTable.setModel(model);
            }
        });

    }
}
