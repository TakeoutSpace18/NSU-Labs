package nsu.urdin.CarFactory;

import lombok.Getter;
import lombok.extern.slf4j.Slf4j;

import java.util.ArrayList;
import java.util.List;

@Slf4j
public class Storage<T> {
    private String name;
    private List<T> items;
    @Getter
    private final int capacity;
    public Storage(String name, int capacity) {
        this.name = name;
        this.capacity = capacity;
        items = new ArrayList<>(capacity);

        log.debug("Created \"{}\" storage, capacity = {}", name, capacity);
    }

    public int getItemCount() {
        return items.size();
    }

    public synchronized void putItem(T item) {
        while (items.size() >= capacity) {
            try {
                wait();
            } catch (InterruptedException e) {
                log.error("Wait interrupted while putting item to \"{}\" storage", name);
            }
        }
        items.add(item);
        notify();

        log.debug("Put {} to {}, current load: {}/{} items", item, name, getItemCount(), getCapacity());
    }

    public synchronized T getItem() {
        while (items.isEmpty()) {
            try {
                wait();
            } catch (InterruptedException e) {
                log.error("Wait interrupted while getting item from \"{}\" storage", name);
            }
        }
        T result = items.remove(items.size() - 1);
        notify();

        log.debug("Got {} from {}, current load: {}/{} items", result, name, getItemCount(), getCapacity());
        return result;
    }
}
