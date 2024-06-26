package nsu.urdin.CarFactory.ui;

import javafx.application.Platform;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.scene.control.ProgressBar;
import javafx.scene.control.Slider;
import javafx.scene.control.TextField;
import javafx.scene.text.Text;
import nsu.urdin.CarFactory.Utils;
import nsu.urdin.CarFactory.storage.StorageListener;

public abstract class ComponentInfoPanelController implements StorageListener {

    private static final long UI_UPDATE_DELAY = 100;
    @FXML
    private Slider timeInput;

    @FXML
    private ProgressBar loadPercentageProgressBar;

    @FXML
    private Text producedCountText;

    @FXML
    private TextField storageCapacityInput;

    @FXML
    private Text storageStatusText;

    @FXML
    private TextField countInput;

    private long lastUpdateTime;

    @FXML
    void onTimeInputAction(Number value) {
        setComponentFabricationTime(value.intValue());
    }


    @FXML
    void onCapacityInputAction(ActionEvent event) {
        try {
            int capacity = Utils.ParseNonNegativeInteger(storageCapacityInput.getText());
            setStorageCapacity(capacity);
        } catch (NumberFormatException e) {
            storageCapacityInput.setText(String.valueOf(getStorageCapacity()));
        }
    }

    @FXML
    void onCountInputAction(ActionEvent event) {
        try {
            int count = Utils.ParseNonNegativeInteger(countInput.getText());
            setEntityCount(count);
        } catch (NumberFormatException e) {
            countInput.setText(String.valueOf(getEntityCount()));
        }
    }


    @FXML
    protected void initialize() {
        lastUpdateTime = System.currentTimeMillis();
        timeInput.setValue(getComponentFabricationTime());
        storageCapacityInput.setText(String.valueOf(getStorageCapacity()));
        storageStatusText.setText("(" + getStorageItemsCount() + "/" + getStorageCapacity() + ")");
        if (countInput != null) {
            countInput.setText(String.valueOf(getEntityCount()));
        }
        loadPercentageProgressBar.setProgress((double) getStorageItemsCount() / getStorageCapacity());
        producedCountText.setText(String.valueOf(getTotalProducedComponents()));
        timeInput.valueProperty().addListener((observable, oldValue, newValue) -> onTimeInputAction(newValue));
    }

    @Override
    public void onStorageStateChange(int itemsCount, int capacity) {
        long currentTime = System.currentTimeMillis();
        if (currentTime - lastUpdateTime > UI_UPDATE_DELAY) {
            lastUpdateTime = currentTime;
            Platform.runLater(() -> {
                storageStatusText.setText("(" + itemsCount + "/" + capacity + ")");
                loadPercentageProgressBar.setProgress((double) itemsCount / capacity);
                producedCountText.setText(String.valueOf(getTotalProducedComponents()));
            });
        }
    }

    protected abstract void setComponentFabricationTime(int fabricationTime);

    protected abstract void setStorageCapacity(int capacity);

    protected abstract void setEntityCount(int suppliersCount);

    protected abstract int getStorageCapacity();

    protected abstract int getStorageItemsCount();

    protected abstract int getComponentFabricationTime();

    protected abstract int getEntityCount();

    protected abstract int getTotalProducedComponents();
}
