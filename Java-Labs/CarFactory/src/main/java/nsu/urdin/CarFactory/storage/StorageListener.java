package nsu.urdin.CarFactory.storage;

public interface StorageListener {
    void onStorageStateChange(int itemsCount, int capacity);
}
