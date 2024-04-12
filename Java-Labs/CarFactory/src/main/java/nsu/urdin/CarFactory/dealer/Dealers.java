package nsu.urdin.CarFactory.dealer;

import lombok.Getter;
import nsu.urdin.CarFactory.CarFactoryConfig;
import nsu.urdin.CarFactory.entity.components.Accessories;
import nsu.urdin.CarFactory.storage.Storages;
import nsu.urdin.CarFactory.supplier.Supplier;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class Dealers {
    Storages storages;
    CarFactoryConfig config;

    @Getter
    private int carRequestDelay;
    List<Dealer> dealers;

    ExecutorService workers;

    public Dealers(Storages storages, CarFactoryConfig config) {
        this.storages = storages;
        this.config = config;

        dealers = new ArrayList<>();
        carRequestDelay = config.getDealerCarRequestDelay();

        for (int i = 0; i < config.getDealersNumber(); ++i) {
            String name = "Dealer_" + i;
            dealers.add(new Dealer(name, storages.getFinishedCars(), carRequestDelay));
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
            tail.forEach(Dealer::stop);
            tail.clear();
        }
        else {
            for (int i = dealers.size(); i < dealersCount; ++i) {
                String name = "Dealer_" + i;
                dealers.add(new Dealer(name, storages.getFinishedCars(), carRequestDelay));
                workers.execute(dealers.get(i));
            }
        }
    }

    public int getTotalCarsSold() {
        return dealers.stream().mapToInt(Dealer::getTotalCarsSold).sum();
    }
}
