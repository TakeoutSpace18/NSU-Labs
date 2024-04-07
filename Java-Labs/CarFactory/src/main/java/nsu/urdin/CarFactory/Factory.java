package nsu.urdin.CarFactory;

import lombok.Getter;
import nsu.urdin.CarFactory.components.Accessories;
import nsu.urdin.CarFactory.components.Body;
import nsu.urdin.CarFactory.components.Engine;
import org.springframework.stereotype.Service;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

@Service
public class Factory {
    CarFactoryConfig config;
    @Getter
    Storages storages;
    ExecutorService workersPool;
    Thread engineSupplier;
    Thread bodySupplier;
    ExecutorService accessorySuppliers;
    ExecutorService dealers;
    public Factory() {
        config = new CarFactoryConfig();
        storages = new Storages();
        workersPool = Executors.newFixedThreadPool(config.getWorkersNumber());

        createSuppliers();
        createDealers();
    }

    private void createDealers() {
        dealers = Executors.newFixedThreadPool(config.getDealersNumber());
        for (int i = 0; i < config.getDealersNumber(); ++i) {
            String name = "Dealer_" + i;
            accessorySuppliers.submit(new Dealer(name, storages.getFinishedCars(), config.getDealerCarRequestDelay()));
        }
    }

    private void createSuppliers() {
        engineSupplier = new Thread(new Supplier<>(storages.getEngines(), config.getEngineFabricationTime(), Engine.class, "engineSupplier"));
        engineSupplier.start();

        bodySupplier = new Thread(new Supplier<>(storages.getBodies(), config.getBodyFabricationTime(), Body.class, "bodySupplier"));
        bodySupplier.start();

        accessorySuppliers = Executors.newFixedThreadPool(config.getAccessorySuppliersNumber());
        for (int i = 0; i < config.getAccessorySuppliersNumber(); ++i) {
            String name = "AccessorySupplier_" + i;
            accessorySuppliers.submit(new Supplier<>(storages.getAccessories(), config.getAccessoryFabricationTime(), Accessories.class, name));
        }
    }

    public void requestCarAssembly(int carsCount) {
        for (int i = 0; i < carsCount; ++i) {
            workersPool.submit(new AssembleTask(storages));
        }
    }

}
