package nsu.urdin.CarFactory.storage;

import lombok.Getter;
import lombok.extern.slf4j.Slf4j;

import java.util.ArrayList;
import java.util.List;

@Slf4j
public class Storage<T> {
    private String name;
    private ArrayList<T> items;
    @Getter
    private int capacity;

    private List<StorageListener> listeners;

    public Storage(String name, int capacity) {
        this.name = name;
        this.capacity = capacity;
        items = new ArrayList<>(capacity);
        listeners = new ArrayList<>();

        log.debug("Created \"{}\" storage, capacity = {}", name, capacity);
    }

    public void setCapacity(int newCapacity) {
        if (newCapacity < capacity) {
            items.subList(newCapacity, items.size()).clear();
        }
        items.ensureCapacity(newCapacity);
        this.capacity = newCapacity;
        listeners.forEach(listener -> listener.onStorageStateChange(getItemsCount(), getCapacity()));
    }

    public int getItemsCount() {
        return items.size();
    }

    public void addListener(StorageListener listener) {
        listeners.add(listener);
        listener.onStorageStateChange(getItemsCount(), getCapacity());
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
        listeners.forEach(listener -> listener.onStorageStateChange(getItemsCount(), getCapacity()));
        log.debug("Put {} to {}, current load: {}/{} items", item, name, getItemsCount(), getCapacity());

        notify();
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
        listeners.forEach(listener -> listener.onStorageStateChange(getItemsCount(), getCapacity()));
        log.debug("Got {} from {}, current load: {}/{} items", result, name, getItemsCount(), getCapacity());

        notify();
        return result;
    }
}
