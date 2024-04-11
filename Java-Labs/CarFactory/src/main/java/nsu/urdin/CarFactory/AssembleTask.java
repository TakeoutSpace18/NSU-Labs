package nsu.urdin.CarFactory;

import lombok.extern.slf4j.Slf4j;
import nsu.urdin.CarFactory.entity.Car;
import nsu.urdin.CarFactory.storage.Storages;

@Slf4j
public class AssembleTask implements Runnable {

    private Storages storages;
    public AssembleTask(Storages storages) {
        this.storages = storages;
    }

    @Override
    public void run() {
        Car producedCar = Car.builder()
                .accessories(storages.getAccessories().getItem())
                .body(storages.getBodies().getItem())
                .engine(storages.getEngines().getItem())
                .build();

        storages.getFinishedCars().putItem(producedCar);
    }
}
