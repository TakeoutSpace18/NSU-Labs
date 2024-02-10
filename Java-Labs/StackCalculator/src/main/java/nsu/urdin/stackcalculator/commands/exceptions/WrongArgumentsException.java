package nsu.urdin.stackcalculator.commands.exceptions;

import nsu.urdin.stackcalculator.exceptions.StackCalculatorException;

import java.text.MessageFormat;

public class WrongArgumentsException extends StackCalculatorException {
    public WrongArgumentsException(String message) {
        super(message);
    }
}
