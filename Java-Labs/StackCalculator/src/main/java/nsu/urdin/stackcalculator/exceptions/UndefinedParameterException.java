package nsu.urdin.stackcalculator.exceptions;

public class UndefinedParameterException extends StackCalculatorException {
    public UndefinedParameterException(String name) {
        super(String.format("Parameter with name \"%s\" is undefined!", name));
    }
}
