package nsu.urdin.CarFactory;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.event.EventListener;

public class FinishedCarsStorageController {


    @EventListener
    void handleCarSell(CarSellEvent event) {

    }

}
