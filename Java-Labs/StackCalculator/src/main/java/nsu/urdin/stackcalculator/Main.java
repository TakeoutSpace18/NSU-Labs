package nsu.urdin.stackcalculator;

import java.io.File;
import java.io.IOException;

public class Main {
    public static void main(String[] args) throws IOException {
        System.out.println(new File(".").getAbsolutePath());


        StackCalculator calc = new StackCalculator(args);
        calc.run();
    }
}

