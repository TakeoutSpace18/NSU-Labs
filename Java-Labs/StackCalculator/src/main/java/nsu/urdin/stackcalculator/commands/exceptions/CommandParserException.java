package nsu.urdin.stackcalculator.commands.exceptions;

import nsu.urdin.stackcalculator.exceptions.StackCalculatorException;

public class CommandParserException extends StackCalculatorException {
    public CommandParserException(String message) {
        super(message);
    }
}
