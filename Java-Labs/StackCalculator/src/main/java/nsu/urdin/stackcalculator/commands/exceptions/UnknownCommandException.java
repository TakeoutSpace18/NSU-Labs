package nsu.urdin.stackcalculator.commands.exceptions;

public class UndefinedCommandException extends RuntimeException {
    public UndefinedCommandException(String message) {
        super(message);
    }
}
