package nsu.urdin.CarFactory;

import org.springframework.boot.context.properties.ConfigurationProperties;
import org.springframework.validation.annotation.Validated;

@Validated
@ConfigurationProperties(prefix = "carfactory")
public record CarFactoryConfig (
    int engineStorageCapacity,
    int engineFabricationTime,

    int bodyStorageCapacity,
    int bodyFabricationTime,

    int accessoriesStorageCapacity,
    int accessoriesFabricationTime,
    int accessoriesSuppliersCount,

    int finishedCarsStorageCapacity,

    int workersCount,

    int dealersCount,
    int dealerCarRequestDelay
) {}
