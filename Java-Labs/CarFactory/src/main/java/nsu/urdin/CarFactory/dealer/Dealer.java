package nsu.urdin.CarFactory.dealer;

import lombok.Getter;
import lombok.Setter;
import lombok.extern.slf4j.Slf4j;
import nsu.urdin.CarFactory.entity.Car;
import nsu.urdin.CarFactory.storage.CarStorage;

import java.util.Random;
import java.util.concurrent.atomic.AtomicBoolean;

@Slf4j
public class Dealer implements Runnable {
    private final CarStorage carStorage;
    @Setter
    private int carRequestDelay;
    private final String name;
    private AtomicBoolean isRunning;
    @Getter
    private int totalCarsSold;

    public Dealer(String name, CarStorage storage, int carRequestDelay) {
        this.carStorage = storage;
        this.carRequestDelay = carRequestDelay;
        this.name = name;
        this.isRunning = new AtomicBoolean(false);
        this.totalCarsSold = 0;

        log.debug("Created \"{}\" dealer, carRequestDelay = {}", name, carRequestDelay);
    }

    @Override
    public void run() {
        isRunning.set(true);

        try {
            // initial delay to randomize among other dealers
            Thread.sleep(new Random().nextInt(carRequestDelay));
            while (true) {
                Car car = carStorage.getItem();
                totalCarsSold++;
                log.info("Dealer \"{}\": Auto {} \n\tBody: {},\n\tEngine: {}\n\tAccessory: {})",
                        name, car.getId(), car.getBody().getId(),
                        car.getEngine().getId(), car.getAccessories().getId());
                Thread.sleep(carRequestDelay);
            }
        } catch (InterruptedException e) {
            log.info("Dealer \"{}\" was interrupted", name);
        }
    }

    public void stop() {
        isRunning.set(false);
    }
}
