package nsu.urdin.CarFactory;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.ApplicationEventPublisher;

public class FinishedCarsStorage extends Storage<Car> {
    @Autowired
    ApplicationEventPublisher eventPublisher;
    public FinishedCarsStorage(int capacity) {
        super("finishedCarsStorage", capacity);
    }

    @Override
    public synchronized void putItem(Car item) throws InterruptedException {
        super.putItem(item);
        eventPublisher.publishEvent(new CarSellEvent());
    }

    @Override
    public synchronized Car getItem() throws InterruptedException {
        return super.getItem();
    }
}
