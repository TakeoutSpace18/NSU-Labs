package nsu.urdin.CarFactory.supplier;

import lombok.AllArgsConstructor;
import lombok.Getter;
import lombok.extern.slf4j.Slf4j;
import nsu.urdin.CarFactory.entity.CarComponent;
import nsu.urdin.CarFactory.storage.Storage;

import java.lang.reflect.InvocationTargetException;
import java.util.Random;

@AllArgsConstructor
@Slf4j
public class Supplier<T extends CarComponent> implements Runnable {
    private Storage<T> targetStorage;
    @Getter
    private int fabricationTime;
    Class<T> componentClass;
    private final String name;

    @Override
    public void run() {
        try {
            // initial delay to randomize among other suppliers
            Thread.sleep(new Random().nextInt(fabricationTime));
            while (true) {
                Thread.sleep(fabricationTime);
                targetStorage.putItem(componentClass.getDeclaredConstructor().newInstance());
            }
        } catch (InterruptedException e) {
            log.info("Supplier \"{}\" was interrupted", name);
        } catch (InstantiationException | IllegalAccessException | InvocationTargetException | NoSuchMethodException e) {
            e.printStackTrace();
            Thread.currentThread().interrupt();
        }
    }

    public void setFabricationTime(int fabricationTime) {
        log.info("Setting {} fabrication time to {}", name, fabricationTime);
        this.fabricationTime = fabricationTime;
    }
}
