package nsu.urdin.stackcalculator;

import nsu.urdin.stackcalculator.exceptions.UndefinedParameterException;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.io.PrintStream;
import java.util.HashMap;
import java.util.Map;
import java.util.Stack;

public class CalcContext {
    private final Map<String, Double> parameters;
    private final Stack<Double> stack;
    private final PrintStream outputCLIStream;

    public CalcContext() {
        this.stack = new Stack<>();
        this.parameters = new HashMap<>();
        this.outputCLIStream = System.out;
    }

    public double pushValue(double val) {
        return stack.push(val);
    }

    public double popValue()  {
        return stack.pop();
    }

    public int stackSize() {
        return stack.size();
    }

    public boolean isStackEmpty() {
        return stack.isEmpty();
    }

    public double peekValue() {
        return stack.peek();
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

    public Double addParameter(String name, Double value) {
        return parameters.put(name, value);
    }
}
