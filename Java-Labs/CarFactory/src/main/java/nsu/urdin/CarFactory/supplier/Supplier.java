package nsu.urdin.CarFactory.supplier;

import lombok.Getter;
import lombok.extern.slf4j.Slf4j;
import nsu.urdin.CarFactory.entity.components.CarComponent;
import nsu.urdin.CarFactory.storage.Storage;

import java.lang.reflect.InvocationTargetException;
import java.util.Random;
import java.util.concurrent.atomic.AtomicBoolean;

@Slf4j
public class Supplier<T extends CarComponent> implements Runnable {
    private final Storage<T> targetStorage;
    @Getter private int fabricationTime;
    private final String name;
    @Getter private int totalComponentsProduced;
    private AtomicBoolean isRunning;

    Class<T> componentClass;

    public Supplier(Storage<T> targetStorage, int fabricationTime, Class<T> componentClass, String name) {
        this.name = name;
        this.componentClass = componentClass;
        this.fabricationTime = fabricationTime;
        this.targetStorage = targetStorage;
        this.totalComponentsProduced = 0;
        this.isRunning = new AtomicBoolean(false);

        log.debug("Created \"{}\" supplier, fabricationTime = {}", name, fabricationTime);
    }

    @Override
    public void run() {
        isRunning = new AtomicBoolean(true);

        try {
            // initial delay to randomize among other suppliers
            Thread.sleep(new Random().nextInt(fabricationTime));
            while (isRunning.get()) {
                Thread.sleep(fabricationTime);
                totalComponentsProduced++;
                targetStorage.putItem(componentClass.getDeclaredConstructor().newInstance());
            }
        } catch (InterruptedException e) {
            log.info("Supplier \"{}\" was interrupted", name);
            Thread.currentThread().interrupt();
        } catch (InstantiationException | IllegalAccessException | InvocationTargetException | NoSuchMethodException e) {
            e.printStackTrace();
            Thread.currentThread().interrupt();
        }
    }

    public void setFabricationTime(int fabricationTime) {
        log.debug("Setting {} fabrication time to {}", name, fabricationTime);
        this.fabricationTime = fabricationTime;
    }

    public void stop() {
        isRunning.set(false);
    }
}
