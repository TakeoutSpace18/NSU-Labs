package nsu.urdin.CarFactory;

import jakarta.validation.constraints.PositiveOrZero;
import org.springframework.boot.context.properties.ConfigurationProperties;
import org.springframework.validation.annotation.Validated;

@Validated
@ConfigurationProperties(prefix = "carfactory")
public record CarFactoryConfig (
    @PositiveOrZero int engineStorageCapacity,
    @PositiveOrZero int engineFabricationTime,

    @PositiveOrZero int bodyStorageCapacity,
    @PositiveOrZero int bodyFabricationTime,

    @PositiveOrZero int accessoriesStorageCapacity,
    @PositiveOrZero int accessoriesFabricationTime,
    @PositiveOrZero int accessoriesSuppliersCount,

    @PositiveOrZero int finishedCarsStorageCapacity,

    @PositiveOrZero int workersCount,

    @PositiveOrZero int dealersCount,
    @PositiveOrZero int dealerCarRequestDelay
) {}
