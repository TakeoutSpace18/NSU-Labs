package nsu.urdin.CarFactory.entity;

import lombok.Builder;
import lombok.Getter;
import nsu.urdin.CarFactory.entity.components.Accessories;
import nsu.urdin.CarFactory.entity.components.Body;
import nsu.urdin.CarFactory.entity.components.Engine;

import java.util.UUID;

@Getter
public class Car {
    private final UUID id;

    private Engine engine;
    private Body body;
    private Accessories accessories;

    @Builder
    public Car(Engine engine, Body body, Accessories accessories) {
        id = UUID.randomUUID();
        this.engine = engine;
        this.body = body;
        this.accessories = accessories;
    }

    @Override
    public String toString() {
        return "Car(" + getId() + ")";
    }
}
