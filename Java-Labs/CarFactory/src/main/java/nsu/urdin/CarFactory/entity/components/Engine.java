package nsu.urdin.CarFactory.entity.components;

import nsu.urdin.CarFactory.entity.CarComponent;

public class Engine extends CarComponent {
    @Override
    public String toString() {
        return "Engine(" + getId() + ")";
    }
}
