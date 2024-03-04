package nsu.urdin.stackcalculator;

import java.io.IOException;

public class Main {
    public static void main(String[] args) throws IOException {
        StackCalculator calc = new StackCalculator(args);
        calc.run();
    }
}

