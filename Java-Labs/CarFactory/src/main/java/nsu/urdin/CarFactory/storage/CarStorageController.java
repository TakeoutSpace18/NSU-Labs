package nsu.urdin.CarFactory.storage;

import jakarta.annotation.PostConstruct;
import lombok.extern.slf4j.Slf4j;
import nsu.urdin.CarFactory.FactoryService;
import nsu.urdin.CarFactory.events.CarSellEvent;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.event.EventListener;
import org.springframework.stereotype.Controller;

@Controller
@Slf4j
public class CarStorageController {

    @Autowired
    FactoryService factoryService;


    @EventListener
    void handleCarSell(CarSellEvent event) {
        requestStorageFillup();
    }

    @PostConstruct
    void requestStorageFillup() {
        var finishedCarsStorage = factoryService.getStorages().getFinishedCars();
        int carsToRequest = finishedCarsStorage.getCapacity() - finishedCarsStorage.getItemsCount();
        factoryService.requestCarAssembly(carsToRequest);
        log.debug("Requesting assembly of {} cars", carsToRequest);
    }

}
