package nsu.urdin.CarFactory.entity.components;

import nsu.urdin.CarFactory.entity.CarComponent;

public class Accessories extends CarComponent {
    @Override
    public String toString() {
        return "Accessories(" + getId() + ")";
    }
}
