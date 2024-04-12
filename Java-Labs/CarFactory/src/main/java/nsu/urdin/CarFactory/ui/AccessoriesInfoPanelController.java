package nsu.urdin.CarFactory.ui;

import net.rgielen.fxweaver.core.FxmlView;
import nsu.urdin.CarFactory.FactoryService;
import org.springframework.stereotype.Component;

@FxmlView("accessories-info-panel.fxml")
@Component
public class AccessoriesInfoPanelController extends ComponentInfoPanelController {

    final FactoryService factoryService;

    public AccessoriesInfoPanelController(FactoryService factoryService) {
        this.factoryService = factoryService;
    }

    @Override
    protected void initialize() {
        super.initialize();
        factoryService.getStoragesController().getAccessories().addListener(this);
    }

    @Override
    protected void setComponentFabricationTime(int fabricationTime) {
        factoryService.getSuppliersController().setAccessoriesFabricationTime(fabricationTime);
    }

    @Override
    protected void setStorageCapacity(int capacity) {
        factoryService.getStoragesController().getAccessories().setCapacity(capacity);
    }

    @Override
    protected void setSuppliersCount(int suppliersCount) {
        factoryService.getSuppliersController().setAccessoriesSuppliersCount(suppliersCount);
    }

    @Override
    protected int getStorageCapacity() {
        return factoryService.getStoragesController().getAccessories().getCapacity();
    }

    @Override
    protected int getStorageItemsCount() {
        return factoryService.getStoragesController().getAccessories().getItemsCount();
    }

    @Override
    protected int getComponentFabricationTime() {
        return factoryService.getSuppliersController().getAccessoriesFabricationTime();
    }

    @Override
    protected int getSuppliersCount() {
        return factoryService.getSuppliersController().getAccessoriesSuppliersCount();
    }

    @Override
    protected int getTotalProducedComponents() {
        return factoryService.getSuppliersController().getTotalAccessoriesProduced();
    }
}
