package nsu.urdin.CarFactory.dealer;

import lombok.Getter;
import nsu.urdin.CarFactory.CarFactoryConfig;
import nsu.urdin.CarFactory.storage.StoragesController;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class DealersController {
    StoragesController storagesController;
    CarFactoryConfig config;

    @Getter
    private int carRequestDelay;
    private int carsSoldByDeletedDealers;
    List<Dealer> dealers;

    ExecutorService workers;

    public DealersController(StoragesController storagesController, CarFactoryConfig config) {
        this.storagesController = storagesController;
        this.config = config;

        dealers = new ArrayList<>();
        carRequestDelay = config.dealerCarRequestDelay();
        carsSoldByDeletedDealers = 0;

        for (int i = 0; i < config.dealersCount(); ++i) {
            String name = "Dealer_" + i;
            dealers.add(new Dealer(name, storagesController.getFinishedCars(), carRequestDelay));
        }
    }

    public void start() {
        workers = Executors.newCachedThreadPool();
        dealers.forEach(dealer -> workers.execute(dealer));
    }

    public void setCarRequestDelay(int time) {
        carRequestDelay = time;
        dealers.forEach(dealer -> dealer.setCarRequestDelay(time));
    }

    public int getDealersCount() {
        return dealers.size();
    }

    public void setDealersCount(int dealersCount) {
        if (dealersCount < dealers.size()) {
            List<Dealer> tail = dealers.subList(dealersCount, dealers.size());
            tail.forEach(dealer -> carsSoldByDeletedDealers += dealer.getTotalCarsSold());
            tail.forEach(Dealer::stop);
            tail.clear();
        }
        else {
            for (int i = dealers.size(); i < dealersCount; ++i) {
                String name = "Dealer_" + i;
                dealers.add(new Dealer(name, storagesController.getFinishedCars(), carRequestDelay));
                workers.execute(dealers.get(i));
            }
        }
    }

    public int getTotalCarsSold() {
        return carsSoldByDeletedDealers + dealers.stream().mapToInt(Dealer::getTotalCarsSold).sum();
    }
}
