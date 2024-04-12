package nsu.urdin.CarFactory.storage;

import lombok.extern.slf4j.Slf4j;
import nsu.urdin.CarFactory.FactoryService;
import org.springframework.stereotype.Component;

@Component
@Slf4j
public class CarStorageController implements StorageListener{

    FactoryService factoryService;

    public CarStorageController(FactoryService factoryService) {
        this.factoryService = factoryService;
        factoryService.getStoragesController().getFinishedCars().addListener(this);
    }

    @Override
    public void onStorageStateChange(int itemsCount, int capacity) {
        int carsToRequest = (int) Math.ceil((capacity - itemsCount) * 0.75) - factoryService.getPendingAssemblyTasksCount();
        if (carsToRequest > 0) {
            factoryService.requestCarAssembly(carsToRequest);
            log.debug("Requesting assembly of {} cars", carsToRequest);
        }
    }
}
