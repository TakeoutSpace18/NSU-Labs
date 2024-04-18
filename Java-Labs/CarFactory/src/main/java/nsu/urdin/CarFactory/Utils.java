package nsu.urdin.CarFactory;

public class Utils {

    static public int ParseNonNegativeInteger(String str) {
        int val = Integer.parseInt(str);
        if (val < 0) {
            throw new NumberFormatException("Value must be a positive integer");
        }

        return val;
    }
}
