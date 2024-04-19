package nsu.urdin.CarFactory.ui;

import javafx.application.Application;
import javafx.application.Platform;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;
import net.rgielen.fxweaver.core.FxWeaver;
import nsu.urdin.CarFactory.CarFactorySpringBootApplication;
import nsu.urdin.CarFactory.FactoryService;
import org.springframework.boot.builder.SpringApplicationBuilder;
import org.springframework.context.ConfigurableApplicationContext;

public class CarFactoryJavaFXApplication extends Application {
    private ConfigurableApplicationContext applicationContext;


    @Override
    public void init() {
        String[] args = getParameters().getRaw().toArray(new String[0]);

        this.applicationContext = new SpringApplicationBuilder()
                .sources(CarFactorySpringBootApplication.class)
                .run(args);
    }

    @Override
    public void stop() throws Exception {
        FactoryService factoryService = applicationContext.getBean(FactoryService.class);
        factoryService.shutdown();
        this.applicationContext.close();
        Platform.exit();
    }

    @Override
    public void start(Stage primaryStage) {
        FxWeaver fxWeaver = applicationContext.getBean(FxWeaver.class);
        Parent root = fxWeaver.loadView(MainPanelController.class);
        Scene scene = new Scene(root);
        primaryStage.setScene(scene);

        primaryStage.show();
    }
}
