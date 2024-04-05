package nsu.urdin.CarFactory;

import lombok.AllArgsConstructor;
import lombok.Setter;
import nsu.urdin.CarFactory.components.CarComponent;

import java.lang.reflect.InvocationTargetException;

@AllArgsConstructor
public class CarComponentSupplier<T extends CarComponent> implements Runnable {
    private Storage<T> targetStorage;
    @Setter
    private int fabricationTime;
    Class<T> componentClass;

    @Override
    public void run() {
        while (true) {
            try {
                Thread.sleep(fabricationTime);
                targetStorage.putItem(componentClass.getDeclaredConstructor().newInstance());
            } catch (InterruptedException | InstantiationException | IllegalAccessException |
                     InvocationTargetException | NoSuchMethodException e) {
                e.printStackTrace();
                Thread.currentThread().interrupt();
            }
        }
    }
}
