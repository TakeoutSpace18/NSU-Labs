package nsu.urdin.CarFactory.ui;

import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.scene.control.ProgressBar;
import javafx.scene.control.TextField;
import javafx.scene.text.Text;
import nsu.urdin.CarFactory.storage.StorageListener;

public abstract class ComponentInfoPanelController implements StorageListener {

    @FXML
    private TextField fabricationTimeInput;

    @FXML
    private ProgressBar loadPercentageProgressBar;

    @FXML
    private Text panelTitle;

    @FXML
    private Text producedCountText;

    @FXML
    private TextField storageCapacityInput;

    @FXML
    private Text storageStatusText;

    @FXML
    private TextField suppliersCountInput;


    @FXML
    void onFabricationTimeInputAction(ActionEvent event) {
        try {
            int fabricationTime = parseNonNegativeInteger(fabricationTimeInput.getText());
            setComponentFabricationTime(fabricationTime);
        } catch (NumberFormatException e) {
            fabricationTimeInput.setText(String.valueOf(getComponentFabricationTime()));
        }
    }


    @FXML
    void onCapacityInputAction(ActionEvent event) {
        try {
            int capacity = parseNonNegativeInteger(storageCapacityInput.getText());
            setStorageCapacity(capacity);
        } catch (NumberFormatException e) {
            storageCapacityInput.setText(String.valueOf(getStorageCapacity()));
        }
    }

    @FXML
    void onSuppliersCountInputAction(ActionEvent event) {
        try {
            int suppliersCount = parseNonNegativeInteger(suppliersCountInput.getText());
            setSuppliersCount(suppliersCount);
        } catch (NumberFormatException e) {
            suppliersCountInput.setText(String.valueOf(getSuppliersCount()));
        }
    }


    protected int parseNonNegativeInteger(String str) {
        int val = Integer.parseInt(str);
        if (val < 0) {
            throw new NumberFormatException("Value must be a positive integer");
        }

        return val;
    }

    @FXML
    protected void initialize() {
        fabricationTimeInput.setText(String.valueOf(getComponentFabricationTime()));
        storageCapacityInput.setText(String.valueOf(getStorageCapacity()));
        storageStatusText.setText("(" + getStorageItemsCount() + "/" + getStorageCapacity() + ")");
        if (suppliersCountInput != null) {
            suppliersCountInput.setText(String.valueOf(getSuppliersCount()));
        }
        loadPercentageProgressBar.setProgress((double) getStorageItemsCount() / getStorageCapacity());
        producedCountText.setText(String.valueOf(getTotalProducedComponents()));
    }

    @Override
    public synchronized void onStorageStateChange(int itemsCount, int capacity) {
        storageStatusText.setText("(" + itemsCount + "/" + capacity + ")");
        loadPercentageProgressBar.setProgress((double) itemsCount / capacity);
        storageCapacityInput.setText(String.valueOf(capacity));
        producedCountText.setText(String.valueOf(getTotalProducedComponents()));
    }

    protected abstract void setComponentFabricationTime(int fabricationTime);
    protected abstract void setStorageCapacity(int capacity);
    protected abstract void setSuppliersCount(int suppliersCount);

    protected abstract int getStorageCapacity();
    protected abstract int getStorageItemsCount();
    protected abstract int getComponentFabricationTime();
    protected abstract int getSuppliersCount();
    protected abstract int getTotalProducedComponents();
}
