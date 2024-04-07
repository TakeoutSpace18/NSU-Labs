package nsu.urdin.CarFactory;

import jakarta.annotation.PostConstruct;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.DependsOn;
import org.springframework.context.event.EventListener;
import org.springframework.stereotype.Controller;

@Controller
@Slf4j
public class CarStorageController {

    @Autowired
    Factory factory;


    @EventListener
    void handleCarSell(CarSellEvent event) {
        requestStorageFillup();
    }

    @PostConstruct
    void requestStorageFillup() {
        var finishedCarsStorage = factory.getStorages().getFinishedCars();
        int carsToRequest = finishedCarsStorage.getCapacity() - finishedCarsStorage.getItemCount();
        factory.requestCarAssembly(carsToRequest);
        log.debug("Requesting assembly of {} cars", carsToRequest);
    }

}
