package nsu.urdin.stackcalculator.commands.exceptions;

public class NotEnoughOperandsException extends CommandExecuteException {
    public NotEnoughOperandsException(String rawCommandText, int line) {
        super(rawCommandText, line, "Not enough operands on the stack");
    }
}
