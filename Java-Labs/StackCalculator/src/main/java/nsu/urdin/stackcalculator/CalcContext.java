package nsu.urdin.stackcalculator;

import nsu.urdin.stackcalculator.exceptions.UndefinedParameterException;

import java.util.HashMap;
import java.util.Map;
import java.util.Stack;

public class CalcContext {
    private final Map<String, Double> parameters;
    private final Stack<Double> stack;

    public CalcContext() {
        this.stack = new Stack<Double>();
        this.parameters = new HashMap<String, Double>();
    }

    public Stack<Double> getStack() {
        return stack;
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
