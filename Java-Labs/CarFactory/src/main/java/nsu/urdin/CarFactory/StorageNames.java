package nsu.urdin.CarFactory;

import lombok.Getter;

public enum StorageNames {
    ENGINE("EngineStorage"),
    ACCESSORIES("AccessoriesStorage"),
    BODY("BodyStorage");

    private final String name;

    public String getName() {
        return name;
    }

    StorageNames(String name) {
        this.name = name;
    }
}
