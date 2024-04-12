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
        factoryService.getStoragesController().getBodies().addListener(this);
    }

    @Override
    protected void setComponentFabricationTime(int fabricationTime) {
        factoryService.getSuppliersController().setBodyFabricationTime(fabricationTime);
    }

    @Override
    protected void setStorageCapacity(int capacity) {
        factoryService.getStoragesController().getBodies().setCapacity(capacity);
    }

    @Override
    protected void setSuppliersCount(int suppliersCount) {

    }

    @Override
    protected int getStorageCapacity() {
        return factoryService.getStoragesController().getBodies().getCapacity();
    }

    @Override
    protected int getStorageItemsCount() {
        return factoryService.getStoragesController().getBodies().getItemsCount();
    }

    @Override
    protected int getComponentFabricationTime() {
        return factoryService.getSuppliersController().getBodyFabricationTime();
    }

    @Override
    protected int getSuppliersCount() {
        return 1;
    }

    @Override
    protected int getTotalProducedComponents() {
        return factoryService.getSuppliersController().getTotalBodiesProduced();
    }
}
