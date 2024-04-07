package nsu.urdin.CarFactory;

import lombok.extern.slf4j.Slf4j;

import java.util.Random;

@Slf4j
public class Dealer implements Runnable {
    private final CarStorage carStorage;
    private final int carRequestDelay;
    private final String name;

    public Dealer(String name, CarStorage storage, int carRequestDelay) {
        this.carStorage = storage;
        this.carRequestDelay = carRequestDelay;
        this.name = name;
    }

    @Override
    public void run() {
        try {
            // initial delay to randomize among other dealers
            Thread.sleep(new Random().nextInt(carRequestDelay));
            while (true) {
                carStorage.getItem();
                Thread.sleep(carRequestDelay);
            }
        } catch (InterruptedException e) {
            log.info("Dealer \"{}\" was interrupted", name);
        }
    }
}
