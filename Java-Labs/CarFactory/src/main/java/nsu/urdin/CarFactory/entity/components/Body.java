package nsu.urdin.CarFactory.entity.components;

import nsu.urdin.CarFactory.entity.CarComponent;

public class Body extends CarComponent {
    @Override
    public String toString() {
        return "Body(" + getId() + ")";
    }
}
