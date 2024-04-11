package nsu.urdin.CarFactory.storage;

import nsu.urdin.CarFactory.entity.Car;
import nsu.urdin.CarFactory.events.CarSellEvent;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.ApplicationEventPublisher;

public class CarStorage extends Storage<Car> {
    public CarStorage(int capacity) {
        super("finishedCarsStorage", capacity);
    }

    @Override
    public synchronized void putItem(Car item) {
        super.putItem(item);
    }

    @Override
    public synchronized Car getItem() {
        return super.getItem();
    }
}
