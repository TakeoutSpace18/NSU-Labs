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
        factoryService.getStorages().getAccessories().addListener(this);
    }

    @Override
    protected void setComponentFabricationTime(int fabricationTime) {
        factoryService.getSuppliers().setAccessoriesFabricationTime(fabricationTime);
    }

    @Override
    protected void setStorageCapacity(int capacity) {
        factoryService.getStorages().getAccessories().setCapacity(capacity);
    }

    @Override
    protected void setSuppliersCount(int suppliersCount) {
        factoryService.getSuppliers().setAccessoriesSuppliersCount(suppliersCount);
    }

    @Override
    protected int getStorageCapacity() {
        return factoryService.getStorages().getAccessories().getCapacity();
    }

    @Override
    protected int getStorageItemsCount() {
        return factoryService.getStorages().getAccessories().getItemsCount();
    }

    @Override
    protected int getComponentFabricationTime() {
        return factoryService.getSuppliers().getAccessoriesFabricationTime();
    }

    @Override
    protected int getSuppliersCount() {
        return factoryService.getSuppliers().getAccessoriesSuppliersCount();
    }

    @Override
    protected String getPanelTitle() {
        return "Accessories";
    }
}
