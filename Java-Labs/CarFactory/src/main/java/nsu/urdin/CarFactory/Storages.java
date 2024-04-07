package nsu.urdin.CarFactory;

import lombok.Getter;
import nsu.urdin.CarFactory.components.Accessories;
import nsu.urdin.CarFactory.components.Body;
import nsu.urdin.CarFactory.components.Engine;

@Getter
public class Storages {
    private CarFactoryConfig config;

    private Storage<Engine> engines;
    private Storage<Body> bodies;
    private Storage<Accessories> accessories;
    private CarStorage finishedCars;
    public Storages() {
        // TODO: replace by bean
        config = new CarFactoryConfig();

        engines = new Storage<>("engineStorage", config.getEngineStorageCapacity());
        bodies = new Storage<>("bodyStorage", config.getBodyStorageCapacity());
        accessories = new Storage<>("accessoriesStorage", config.getAccessoryStorageCapacity());

        finishedCars = new CarStorage(config.getFinishedCarsStorageCapacity());
    }
}
