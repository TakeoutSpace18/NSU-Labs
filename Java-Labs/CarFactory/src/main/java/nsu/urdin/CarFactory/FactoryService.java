package nsu.urdin.CarFactory;

import lombok.Getter;
import lombok.extern.slf4j.Slf4j;
import nsu.urdin.CarFactory.dealer.DealersController;
import nsu.urdin.CarFactory.storage.StoragesController;
import nsu.urdin.CarFactory.supplier.SuppliersController;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.concurrent.*;

@Slf4j
@Service
public class FactoryService {
    private final CarFactoryConfig config;

    @Getter
    private final StoragesController storagesController;
    @Getter
    private final SuppliersController suppliersController;
    @Getter
    private final DealersController dealersController;

    ThreadPoolExecutor workersPool;

    @Autowired
    public FactoryService(CarFactoryConfig config) {
        this.config = config;
        storagesController = new StoragesController(config);
        suppliersController = new SuppliersController(storagesController, config);
        dealersController = new DealersController(storagesController, config);
        workersPool = new ThreadPoolExecutor(config.workersCount(),
                Integer.MAX_VALUE, 0L, TimeUnit.MILLISECONDS, new LinkedBlockingQueue<>());

        dealersController.start();
        suppliersController.start();
    }

    public void requestCarAssembly(int carsCount) {
        for (int i = 0; i < carsCount; ++i) {
            workersPool.submit(new AssembleTask(storagesController));
        }
    }

    public int getPendingAssemblyTasksCount() {
        return workersPool.getQueue().size();
    }

    public void shutdown() throws InterruptedException {
        log.info("Shutting down the factory...");
        workersPool.shutdownNow();
        suppliersController.shutdown();
        dealersController.shutdown();
    }

    public int getWorkersCount() {
        return workersPool.getCorePoolSize();
    }

    public void setWorkersCount(int workersCount) {
        workersPool.setCorePoolSize(workersCount);
    }

    public int getBusyWorkersCount() {
        return workersPool.getActiveCount();
    }
}
