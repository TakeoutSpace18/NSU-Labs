package nsu.urdin.CarFactory.storage;

import lombok.Getter;
import nsu.urdin.CarFactory.CarFactoryConfig;
import nsu.urdin.CarFactory.entity.Car;
import nsu.urdin.CarFactory.entity.components.Accessories;
import nsu.urdin.CarFactory.entity.components.Body;
import nsu.urdin.CarFactory.entity.components.Engine;

@Getter
public class StoragesController {
    private final Storage<Engine> engines;
    private final Storage<Body> bodies;
    private final Storage<Accessories> accessories;
    private final Storage<Car> finishedCars;

    public StoragesController(CarFactoryConfig config) {
        engines = new Storage<>("engineStorage", config.engineStorageCapacity());
        bodies = new Storage<>("bodyStorage", config.bodyStorageCapacity());
        accessories = new Storage<>("accessoriesStorage", config.accessoriesStorageCapacity());

        finishedCars = new Storage<>("finishedCarsStorage", config.finishedCarsStorageCapacity());
    }
}
