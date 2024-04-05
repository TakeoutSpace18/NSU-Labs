package nsu.urdin.CarFactory;

import lombok.Builder;
import nsu.urdin.CarFactory.components.Accessories;
import nsu.urdin.CarFactory.components.Body;
import nsu.urdin.CarFactory.components.Engine;

@Builder
public class Car {
    private Engine engine;
    private Body body;
    private Accessories accessories;
}
