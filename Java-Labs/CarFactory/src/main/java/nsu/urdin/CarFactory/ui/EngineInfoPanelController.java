package nsu.urdin.CarFactory.ui;

import net.rgielen.fxweaver.core.FxmlView;
import nsu.urdin.CarFactory.FactoryService;
import org.springframework.stereotype.Component;

@FxmlView("engine-info-panel.fxml")
@Component
public class EngineInfoPanelController extends ComponentInfoPanelController {

    final FactoryService factoryService;

    public EngineInfoPanelController(FactoryService factoryService) {
        this.factoryService = factoryService;
    }

    @Override
    protected void initialize() {
        super.initialize();
        factoryService.getStorages().getEngines().addListener(this);
    }

    @Override
    protected void setComponentFabricationTime(int fabricationTime) {
        factoryService.getSuppliers().setEngineFabricationTime(fabricationTime);
    }

    @Override
    protected void setStorageCapacity(int capacity) {
        factoryService.getStorages().getEngines().setCapacity(capacity);
    }

    @Override
    protected void setSuppliersCount(int suppliersCount) {

    }

    @Override
    protected int getStorageCapacity() {
        return factoryService.getStorages().getEngines().getCapacity();
    }

    @Override
    protected int getStorageItemsCount() {
        return factoryService.getStorages().getEngines().getItemsCount();
    }

    @Override
    protected int getComponentFabricationTime() {
        return factoryService.getSuppliers().getEngineFabricationTime();
    }

    @Override
    protected int getSuppliersCount() {
        return 1;
    }

    @Override
    protected String getPanelTitle() {
        return "Engine";
    }
}
