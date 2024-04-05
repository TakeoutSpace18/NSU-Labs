package nsu.urdin.CarFactory;

import nsu.urdin.CarFactory.components.Accessories;
import nsu.urdin.CarFactory.components.Body;
import nsu.urdin.CarFactory.components.Engine;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.config.ConfigurableBeanFactory;
import org.springframework.boot.SpringApplication;
import org.springframework.context.ApplicationContext;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.context.annotation.Scope;
import org.springframework.core.env.Environment;

@Configuration
public class CarFactoryConfig {

    @Autowired
    private Environment env;

    @Autowired
    private ApplicationContext applicationContext;

    @Bean("engineStorage")
    @Scope("singleton")
    Storage<Engine> getEngineStorage() {
        int capacity = Integer.parseInt(env.getProperty("CarFactory.EngineStorageCapacity"));
        return new Storage<>("engineStorage", capacity);
    }

    @Bean("bodyStorage")
    @Scope("singleton")
    Storage<Body> getBodyStorage() {
        int capacity = Integer.parseInt(env.getProperty("CarFactory.BodyStorageCapacity"));
        return new Storage<>("bodyStorage", capacity);
    }
    @Bean("accessoriesStorage")
    @Scope("singleton")
    Storage<Accessories> getAccessoriesStorage() {
        int capacity = Integer.parseInt(env.getProperty("CarFactory.AccessoriesStorageCapacity"));
        return new Storage<>("accessoriesStorage", capacity);
    }

    @Bean
    @Scope("singleton")
    FinishedCarsStorage getFinishedCarsStorage() {
        int capacity = Integer.parseInt(env.getProperty("CarFactory.FinishedCarsStorageCapacity"));
        return new FinishedCarsStorage(capacity);
    }

    @Bean
    @Scope("singleton")
    CarComponentSupplier<Engine> getEngineSupplier() {
        int fabricationTime = Integer.parseInt(env.getProperty("CarFactory.EngineFabricationTime"));
        return new CarComponentSupplier<>((Storage<Engine>)applicationContext.getBean("engineStorage"), fabricationTime, Engine.class);
    }
}
