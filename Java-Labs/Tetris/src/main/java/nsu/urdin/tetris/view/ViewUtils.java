package nsu.urdin.tetris.view;

import java.awt.*;

public class ViewUtils {
    static void SetEnabledRecursively(Container panel, boolean enabled) {
        panel.setEnabled(enabled);

        for (Component comp : panel.getComponents()) {
            comp.setEnabled(enabled);
            if (comp instanceof Container) {
                SetEnabledRecursively((Container) comp, enabled);
            }
        }
    }
}
