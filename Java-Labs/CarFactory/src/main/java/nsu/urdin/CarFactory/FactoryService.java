package nsu.urdin.CarFactory;

import lombok.Getter;
import nsu.urdin.CarFactory.dealer.Dealer;
import nsu.urdin.CarFactory.dealer.Dealers;
import nsu.urdin.CarFactory.storage.Storages;
import nsu.urdin.CarFactory.supplier.Suppliers;
import org.springframework.stereotype.Service;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

@Service
public class FactoryService {
    CarFactoryConfig config;
    @Getter
    Storages storages;
    @Getter
    Suppliers suppliers;
    @Getter
    Dealers dealers;

    ExecutorService workersPool;

    public FactoryService() {
        config = new CarFactoryConfig();
        storages = new Storages();
        suppliers = new Suppliers(storages, config);
        dealers = new Dealers(storages, config);
        workersPool = Executors.newFixedThreadPool(config.getWorkersNumber());

        suppliers.start();
    }

    public void requestCarAssembly(int carsCount) {
        for (int i = 0; i < carsCount; ++i) {
            workersPool.submit(new AssembleTask(storages));
        }
    }

}
