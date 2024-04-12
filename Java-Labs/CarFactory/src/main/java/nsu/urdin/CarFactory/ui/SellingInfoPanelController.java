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
        factoryService.getStoragesController().getFinishedCars().addListener(this);
    }

    @Override
    protected void setComponentFabricationTime(int fabricationTime) {
        factoryService.getDealersController().setCarRequestDelay(fabricationTime);
    }

    @Override
    protected void setStorageCapacity(int capacity) {
        factoryService.getStoragesController().getFinishedCars().setCapacity(capacity);
    }

    @Override
    protected void setSuppliersCount(int suppliersCount) {
        factoryService.getDealersController().setDealersCount(suppliersCount);
    }

    @Override
    protected int getStorageCapacity() {
        return factoryService.getStoragesController().getFinishedCars().getCapacity();
    }

    @Override
    protected int getStorageItemsCount() {
        return factoryService.getStoragesController().getFinishedCars().getItemsCount();
    }

    @Override
    protected int getComponentFabricationTime() {
        return factoryService.getDealersController().getCarRequestDelay();
    }

    @Override
    protected int getSuppliersCount() {
        return factoryService.getDealersController().getDealersCount();
    }

    @Override
    protected int getTotalProducedComponents() {
        return factoryService.getDealersController().getTotalCarsSold();
    }
}
