package nsu.urdin.CarFactory;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.ApplicationEventPublisher;

public class CarStorage extends Storage<Car> {
    @Autowired
    ApplicationEventPublisher eventPublisher;
    public CarStorage(int capacity) {
        super("finishedCarsStorage", capacity);
    }

    @Override
    public synchronized void putItem(Car item) {
        super.putItem(item);
        eventPublisher.publishEvent(new CarSellEvent());
    }

    @Override
    public synchronized Car getItem() {
        return super.getItem();
    }
}
