package nsu.urdin.CarFactory;

import lombok.extern.slf4j.Slf4j;
import nsu.urdin.CarFactory.entity.Car;
import nsu.urdin.CarFactory.entity.components.Accessories;
import nsu.urdin.CarFactory.entity.components.Body;
import nsu.urdin.CarFactory.entity.components.Engine;
import nsu.urdin.CarFactory.storage.StoragesController;

@Slf4j
public class AssembleTask implements Runnable {

    private final StoragesController storagesController;
    public AssembleTask(StoragesController storagesController) {
        this.storagesController = storagesController;
    }

    @Override
    public void run() {
        Accessories accessories = storagesController.getAccessories().getItem();
        Body body = storagesController.getBodies().getItem();
        Engine engine = storagesController.getEngines().getItem();

        if (accessories == null || body == null || engine == null) {
            // if any component is null then the thread was interrupted, and we need to stop the task
            return;
        }

        Car producedCar = Car.builder()
                .accessories(accessories)
                .body(body)
                .engine(engine)
                .build();

        storagesController.getFinishedCars().putItem(producedCar);
    }
}
