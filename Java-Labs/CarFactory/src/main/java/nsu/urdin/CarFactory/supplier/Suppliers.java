package nsu.urdin.CarFactory.supplier;

import lombok.Getter;
import nsu.urdin.CarFactory.CarFactoryConfig;
import nsu.urdin.CarFactory.FactoryService;
import nsu.urdin.CarFactory.entity.components.Accessories;
import nsu.urdin.CarFactory.entity.components.Body;
import nsu.urdin.CarFactory.entity.components.Engine;
import nsu.urdin.CarFactory.storage.Storages;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class Suppliers {
    Storages storages;
    CarFactoryConfig config;

    Supplier<Engine> engineSupplier;
    Supplier<Body> bodySupplier;

    @Getter
    private int accessoriesFabricationTime;
    List<Supplier<Accessories>> accessoriesSuppliers;

    ExecutorService workers;

    public Suppliers(Storages storages, CarFactoryConfig config) {
        this.storages = storages;
        this.config = config;

        engineSupplier = new Supplier<>(storages.getEngines(), config.getEngineFabricationTime(), Engine.class, "engineSupplier");
        bodySupplier = new Supplier<>(storages.getBodies(), config.getBodyFabricationTime(), Body.class, "bodySupplier");

        accessoriesSuppliers = new ArrayList<>();
        accessoriesFabricationTime = config.getAccessoryFabricationTime();

        for (int i = 0; i < config.getAccessorySuppliersNumber(); ++i) {
            String name = "AccessorySupplier_" + i;
            accessoriesSuppliers.add(new Supplier<>(storages.getAccessories(), accessoriesFabricationTime, Accessories.class, name));
        }
    }

    public void start() {
        workers = Executors.newCachedThreadPool();
        workers.execute(engineSupplier);
        workers.execute(bodySupplier);
        accessoriesSuppliers.forEach(supplier -> workers.execute(supplier));
    }

    public void setEngineFabricationTime(int time) {
        engineSupplier.setFabricationTime(time);
    }

    public void setBodyFabricationTime(int time) {
        bodySupplier.setFabricationTime(time);
    }

    public void setAccessoriesFabricationTime(int time) {
        accessoriesFabricationTime = time;
        accessoriesSuppliers.forEach(supplier -> supplier.setFabricationTime(time));
    }

    public int getEngineFabricationTime() {
        return engineSupplier.getFabricationTime();
    }

    public int getBodyFabricationTime() {
        return bodySupplier.getFabricationTime();
    }

    public int getAccessoriesSuppliersCount() {
        return accessoriesSuppliers.size();
    }

    public void setAccessoriesSuppliersCount(int suppliersCount) {
        if (suppliersCount < accessoriesSuppliers.size()) {
            List<Supplier<Accessories>> tail = accessoriesSuppliers.subList(suppliersCount, accessoriesSuppliers.size());
            tail.forEach(Supplier::stop);
            tail.clear();
        }
        else {
            for (int i = accessoriesSuppliers.size(); i < suppliersCount; ++i) {
                String name = "AccessorySupplier_" + i;
                accessoriesSuppliers.add(new Supplier<>(storages.getAccessories(), accessoriesFabricationTime, Accessories.class, name));
                workers.execute(accessoriesSuppliers.get(i));
            }
        }
    }

    public int getTotalEnginesProduced() {
        return engineSupplier.getTotalComponentsProduced();
    }

    public int getTotalBodyProduced() {
        return bodySupplier.getTotalComponentsProduced();
    }

    public int getTotalAccessoriesProduced() {
        return accessoriesSuppliers.stream().mapToInt(Supplier::getTotalComponentsProduced).sum();
    }
}
