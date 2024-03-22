package nsu.urdin.tetris.view;

import nsu.urdin.tetris.controller.TetrisController;
import nsu.urdin.tetris.model.Scoreboard;
import nsu.urdin.tetris.model.listeners.ScoreboardListener;

import javax.swing.*;
import javax.swing.table.DefaultTableCellRenderer;
import javax.swing.table.DefaultTableModel;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.time.format.DateTimeFormatter;
import java.util.List;

public class JHighScoresPanel {
    private JPanel contentPanel;
    private JTable scoresTable;
    private JButton backButton;
    private JLabel scoreLabel;

    public JHighScoresPanel() {
        scoresTable.getTableHeader().setReorderingAllowed(false);

        backButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                TetrisController.getInstance().mainMenu();
            }
        });

        TetrisController.getInstance().getScoreboard().addListener(new ScoreboardListener() {
            @Override
            public void highScoreChanged(Scoreboard.Entry entry) {
                DateTimeFormatter formatter = DateTimeFormatter.ofPattern("dd-MM-yyyy HH:mm");
                scoreLabel.setText("High score: " + entry.score() + " (" +
                        entry.date().format(formatter) + ")");
            }

            @Override
            public void scoreTableChanged(List<Scoreboard.Entry> scoreTable) {
                DefaultTableModel model = new DefaultTableModel();
                model.addColumn("Date");
                model.addColumn("Score");

                DateTimeFormatter formatter = DateTimeFormatter.ofPattern("dd-MM-yyyy HH:mm");
                scoreTable.forEach(entry -> {
                    model.addRow(new Object[]{entry.date().format(formatter), entry.score()});
                });
                scoresTable.setModel(model);
            }
        });

    }
}
