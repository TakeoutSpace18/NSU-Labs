package nsu.urdin.CarFactory.ui;

import javafx.animation.KeyFrame;
import javafx.animation.Timeline;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.fxml.FXML;
import javafx.scene.control.TextField;
import javafx.scene.layout.TilePane;
import javafx.scene.paint.Color;
import javafx.scene.paint.Paint;
import javafx.scene.shape.Rectangle;
import javafx.util.Duration;
import net.rgielen.fxweaver.core.FxmlView;
import nsu.urdin.CarFactory.FactoryService;
import nsu.urdin.CarFactory.Utils;
import org.springframework.stereotype.Component;


@FxmlView("workers-info-panel.fxml")
@Component
public class WorkersInfoPanelController {
    private static final int INDICATOR_UPDATE_DELAY = 200;

    final FactoryService factoryService;

    @FXML
    private TilePane workersIndicatorPane;

    @FXML
    private TextField workersNumberInput;

    private Timeline indicatorUpdate;

    public WorkersInfoPanelController(FactoryService factoryService) {
        this.factoryService = factoryService;
    }

    @FXML
    void initialize() {
        workersNumberInput.setText(String.valueOf(factoryService.getWorkersCount()));

        this.indicatorUpdate = new Timeline(
                new KeyFrame(Duration.millis(INDICATOR_UPDATE_DELAY),
                        new EventHandler<ActionEvent>() {
                            @Override
                            public void handle(ActionEvent event) {
                                updateWorkerIndicators();
                            }
                        }));
        indicatorUpdate.setCycleCount(Timeline.INDEFINITE);
        indicatorUpdate.play();
    }

    @FXML
    void onWorkersNumberInputAction(ActionEvent event) {
        try {
            int workersCount = Utils.ParseNonNegativeInteger(workersNumberInput.getText());
            factoryService.setWorkersCount(workersCount);
        } catch (NumberFormatException e) {
            workersNumberInput.setText(String.valueOf(factoryService.getWorkersCount()));
        }
    }

    void updateWorkerIndicators() {
        workersIndicatorPane.getChildren().clear();
        for (int i = 0; i < factoryService.getWorkersCount(); i++) {
            Paint color;
            if (i < factoryService.getBusyWorkersCount()) {
                color = Color.RED;
            } else {
                color = Color.GREEN;
            }
            Rectangle rect = new Rectangle(30, 30, color);
            rect.setStroke(Color.BLACK);
            workersIndicatorPane.getChildren().add(rect);
        }
    }

}
