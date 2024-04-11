package nsu.urdin.CarFactory.ui;

import javafx.fxml.FXML;
import net.rgielen.fxweaver.core.FxmlView;
import org.springframework.stereotype.Component;

@Component
@FxmlView("main-panel.fxml")
public class MainPanelController {

    @FXML
    EngineInfoPanelController enginePanelController;
    @FXML
    BodyInfoPanelController bodyPanelController;
    @FXML
    AccessoriesInfoPanelController accessoriesPanelController;
}
