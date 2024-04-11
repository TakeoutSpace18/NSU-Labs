package nsu.urdin.CarFactory.ui;

import net.rgielen.fxweaver.core.FxmlView;
import nsu.urdin.CarFactory.FactoryService;
import org.springframework.stereotype.Component;

@FxmlView("body-info-panel.fxml")
@Component
public class BodyInfoPanelController extends ComponentInfoPanelController {

    final FactoryService factoryService;

    public BodyInfoPanelController(FactoryService factoryService) {
        this.factoryService = factoryService;
    }

    @Override
    protected void initialize() {
        super.initialize();
        factoryService.getStorages().getBodies().addListener(this);
    }

    @Override
    protected void setComponentFabricationTime(int fabricationTime) {
        factoryService.getSuppliers().setBodyFabricationTime(fabricationTime);
    }

    @Override
    protected void setStorageCapacity(int capacity) {
        factoryService.getStorages().getBodies().setCapacity(capacity);
    }

    @Override
    protected void setSuppliersCount(int suppliersCount) {

    }

    @Override
    protected int getStorageCapacity() {
        return factoryService.getStorages().getBodies().getCapacity();
    }

    @Override
    protected int getStorageItemsCount() {
        return factoryService.getStorages().getBodies().getItemsCount();
    }

    @Override
    protected int getComponentFabricationTime() {
        return factoryService.getSuppliers().getBodyFabricationTime();
    }

    @Override
    protected int getSuppliersCount() {
        return 1;
    }

    @Override
    protected String getPanelTitle() {
        return "Body";
    }
}
