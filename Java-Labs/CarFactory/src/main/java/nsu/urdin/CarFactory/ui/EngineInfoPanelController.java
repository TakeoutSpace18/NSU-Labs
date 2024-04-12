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
        factoryService.getStoragesController().getEngines().addListener(this);
    }

    @Override
    protected void setComponentFabricationTime(int fabricationTime) {
        factoryService.getSuppliersController().setEngineFabricationTime(fabricationTime);
    }

    @Override
    protected void setStorageCapacity(int capacity) {
        factoryService.getStoragesController().getEngines().setCapacity(capacity);
    }

    @Override
    protected void setSuppliersCount(int suppliersCount) {

    }

    @Override
    protected int getStorageCapacity() {
        return factoryService.getStoragesController().getEngines().getCapacity();
    }

    @Override
    protected int getStorageItemsCount() {
        return factoryService.getStoragesController().getEngines().getItemsCount();
    }

    @Override
    protected int getComponentFabricationTime() {
        return factoryService.getSuppliersController().getEngineFabricationTime();
    }

    @Override
    protected int getSuppliersCount() {
        return 1;
    }

    @Override
    protected int getTotalProducedComponents() {
        return factoryService.getSuppliersController().getTotalEnginesProduced();
    }
}
