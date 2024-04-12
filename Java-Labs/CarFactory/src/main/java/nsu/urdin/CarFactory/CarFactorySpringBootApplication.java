package nsu.urdin.CarFactory;

import javafx.application.Application;
import net.rgielen.fxweaver.core.FxWeaver;
import net.rgielen.fxweaver.spring.SpringFxWeaver;
import nsu.urdin.CarFactory.ui.CarFactoryJavaFXApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.boot.context.properties.EnableConfigurationProperties;
import org.springframework.context.ConfigurableApplicationContext;
import org.springframework.context.annotation.Bean;

@SpringBootApplication
@EnableConfigurationProperties(CarFactoryConfig.class)
public class CarFactorySpringBootApplication {

	public static void main(String[] args) {
		Application.launch(CarFactoryJavaFXApplication.class, args);

	}

	@Bean
	public FxWeaver fxWeaver(ConfigurableApplicationContext applicationContext) {
		return new SpringFxWeaver(applicationContext);

	}
}
