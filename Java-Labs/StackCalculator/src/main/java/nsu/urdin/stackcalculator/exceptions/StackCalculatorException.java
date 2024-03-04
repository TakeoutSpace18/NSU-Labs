package nsu.urdin.stackcalculator.exceptions;

public class StackCalculatorException extends RuntimeException {
    public StackCalculatorException(String message) {
        super(message);
    }

    public StackCalculatorException(String message, Throwable cause) {
        super(message, cause);
    }
}
