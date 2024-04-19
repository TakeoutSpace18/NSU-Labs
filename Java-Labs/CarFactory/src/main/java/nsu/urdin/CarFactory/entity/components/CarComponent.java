package nsu.urdin.CarFactory.entity.components;

import lombok.Getter;

import java.util.UUID;


@Getter
public abstract class CarComponent {
    private final UUID id;

    public CarComponent() {
        id = UUID.randomUUID();
    }
}
