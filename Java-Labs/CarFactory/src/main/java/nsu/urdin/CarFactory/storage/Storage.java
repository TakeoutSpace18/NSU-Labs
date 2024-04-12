package nsu.urdin.CarFactory.storage;

import lombok.Getter;
import lombok.extern.slf4j.Slf4j;

import java.util.ArrayList;
import java.util.List;

@Slf4j
public class Storage<T> {
    private final String name;
    private final ArrayList<T> items;
    @Getter
    private int capacity;

    private final List<StorageListener> listeners;

    public Storage(String name, int capacity) {
        this.name = name;
        this.capacity = capacity;
        items = new ArrayList<>(capacity);
        listeners = new ArrayList<>();

        log.debug("Created \"{}\" storage, capacity = {}", name, capacity);
    }

    public synchronized void setCapacity(int newCapacity) {
        if (newCapacity < items.size()) {
            items.subList(newCapacity, items.size()).clear();
        }
        items.ensureCapacity(newCapacity);
        this.capacity = newCapacity;
        listeners.forEach(listener -> listener.onStorageStateChange(getItemsCount(), getCapacity()));
        notify();
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
                log.info("Wait interrupted while putting item to \"{}\" storage", name);
                Thread.currentThread().interrupt();
            }
        }
        items.add(item);
        listeners.forEach(listener -> listener.onStorageStateChange(getItemsCount(), getCapacity()));
        log.debug("Put {} to {}, current load: {}/{} items", item, name, getItemsCount(), getCapacity());

        notify();
    }

    public synchronized T getItem() {
        try {
            while (items.isEmpty() && Thread.currentThread().isAlive()) {
                wait();
            }
        } catch (InterruptedException e) {
            log.info("Wait interrupted while getting item from \"{}\" storage", name);
            Thread.currentThread().interrupt();
            return null;
        }

        T result = items.remove(items.size() - 1);
        listeners.forEach(listener -> listener.onStorageStateChange(getItemsCount(), getCapacity()));
        log.debug("Got {} from {}, current load: {}/{} items", result, name, getItemsCount(), getCapacity());

        notify();
        return result;
    }
}
