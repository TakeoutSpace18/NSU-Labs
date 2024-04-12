package nsu.urdin.CarFactory;

import lombok.Getter;

@Getter
public class CarFactoryConfig {
    private int engineStorageCapacity = 10;
    private int engineFabricationTime = 2000;

    private int bodyStorageCapacity = 15;
    private int bodyFabricationTime = 100;

    private int accessoryStorageCapacity = 300;
    private int accessoryFabricationTime = 75;
    private int accessorySuppliersNumber = 1;

    private int finishedCarsStorageCapacity = 10;

    private int workersNumber = 5;

    private int dealersNumber = 3;
    private int dealerCarRequestDelay = 800;

}
