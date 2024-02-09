package nsu.urdin.stackcalculator;

import nsu.urdin.stackcalculator.exceptions.UndefinedParameterException;

import java.io.PrintStream;
import java.util.HashMap;
import java.util.Map;
import java.util.Stack;

public class CalcContext {
    private final
    Map<String, Double> parameters;
    private final Stack<Double> stack;
    private final PrintStream outputCLIStream;

    public CalcContext() {
        this.stack = new Stack<>();
        this.parameters = new HashMap<>();
        this.outputCLIStream = System.out;
    }

    public Stack<Double> getStack() {
        return stack;
    }

    public PrintStream getOutputCLIStream() {
        return outputCLIStream;
    }

    public Double getParameter(String name) throws UndefinedParameterException {
        Double value = parameters.get(name);
        if (value == null) {
            throw new UndefinedParameterException(name);
        }

        return value;
    }

    public void addParameter(String name, Double value) {
        parameters.put(name, value);
    }
}
