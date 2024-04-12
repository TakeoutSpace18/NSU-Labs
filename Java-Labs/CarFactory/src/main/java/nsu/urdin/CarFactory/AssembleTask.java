package nsu.urdin.CarFactory;

import lombok.extern.slf4j.Slf4j;
import nsu.urdin.CarFactory.entity.Car;
import nsu.urdin.CarFactory.storage.StoragesController;

@Slf4j
public class AssembleTask implements Runnable {

    private final StoragesController storagesController;
    public AssembleTask(StoragesController storagesController) {
        this.storagesController = storagesController;
    }

    @Override
    public void run() {
        Car producedCar = Car.builder()
                .accessories(storagesController.getAccessories().getItem())
                .body(storagesController.getBodies().getItem())
                .engine(storagesController.getEngines().getItem())
                .build();

        storagesController.getFinishedCars().putItem(producedCar);
    }
}
