package nsu.urdin.CarFactory.ui;

import net.rgielen.fxweaver.core.FxmlView;
import nsu.urdin.CarFactory.FactoryService;
import org.springframework.stereotype.Component;

@FxmlView("selling-info-panel.fxml")
@Component
public class SellingInfoPanelController extends ComponentInfoPanelController {

    final FactoryService factoryService;

    public SellingInfoPanelController(FactoryService factoryService) {
        this.factoryService = factoryService;
    }

    @Override
    protected void initialize() {
        super.initialize();
        factoryService.getStorages().getEngines().addListener(this);
    }

    @Override
    protected void setComponentFabricationTime(int fabricationTime) {
        factoryService.getDealers().setCarRequestDelay(fabricationTime);
    }

    @Override
    protected void setStorageCapacity(int capacity) {
        factoryService.getStorages().getFinishedCars().setCapacity(capacity);
    }

    @Override
    protected void setSuppliersCount(int suppliersCount) {
        factoryService.getDealers().setDealersCount(suppliersCount);
    }

    @Override
    protected int getStorageCapacity() {
        return factoryService.getStorages().getFinishedCars().getCapacity();
    }

    @Override
    protected int getStorageItemsCount() {
        return factoryService.getStorages().getFinishedCars().getItemsCount();
    }

    @Override
    protected int getComponentFabricationTime() {
        return factoryService.getDealers().getCarRequestDelay();
    }

    @Override
    protected int getSuppliersCount() {
        return factoryService.getDealers().getDealersCount();
    }

    @Override
    protected int getTotalProducedComponents() {
        return factoryService.getDealers().getTotalCarsSold();
    }
}
