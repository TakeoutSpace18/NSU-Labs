package nsu.urdin.CarFactory.components;

import lombok.Getter;

public class Engine extends CarComponent {
    @Override
    public String toString() {
        return "Engine(" + getId() + ")";
    }
}
