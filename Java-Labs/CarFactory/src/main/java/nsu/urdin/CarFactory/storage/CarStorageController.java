package nsu.urdin.CarFactory.storage;

import jakarta.annotation.PostConstruct;
import lombok.extern.slf4j.Slf4j;
import nsu.urdin.CarFactory.FactoryService;
import nsu.urdin.CarFactory.events.CarSellEvent;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.event.EventListener;
import org.springframework.stereotype.Component;

@Component
@Slf4j
public class CarStorageController implements StorageListener{

    FactoryService factoryService;

    public CarStorageController(FactoryService factoryService) {
        this.factoryService = factoryService;
        factoryService.getStorages().getFinishedCars().addListener(this);
    }

    @Override
    public void onStorageStateChange(int itemsCount, int capacity) {
        int carsToRequest = (int) Math.ceil((capacity - itemsCount) * 0.75);
        factoryService.requestCarAssembly(carsToRequest);
        log.debug("Requesting assembly of {} cars", carsToRequest);
    }
}
