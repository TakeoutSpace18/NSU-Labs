package nsu.urdin.CarFactory;

import lombok.Getter;
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
    ExecutorService workersPool;
    ExecutorService dealers;

    public FactoryService() {
        config = new CarFactoryConfig();
        storages = new Storages();
        suppliers = new Suppliers(storages, config);
        workersPool = Executors.newFixedThreadPool(config.getWorkersNumber());

        createDealers();
        suppliers.start();
    }

    private void createDealers() {
        dealers = Executors.newFixedThreadPool(config.getDealersNumber());
        for (int i = 0; i < config.getDealersNumber(); ++i) {
            String name = "Dealer_" + i;
            dealers.execute(new Dealer(name, storages.getFinishedCars(), config.getDealerCarRequestDelay()));
        }
    }



    public void requestCarAssembly(int carsCount) {
        for (int i = 0; i < carsCount; ++i) {
            workersPool.submit(new AssembleTask(storages));
        }
    }

}
