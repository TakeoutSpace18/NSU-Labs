package nsu.urdin.stackcalculator.commands.exceptions;

import nsu.urdin.stackcalculator.exceptions.StackCalculatorException;

import java.text.MessageFormat;

public class CommandArgumentsFormatException extends StackCalculatorException {
    public CommandArgumentsFormatException(String rawCommandText, int line, String reason) {
        super(MessageFormat.format("Invalid arguments format in command \"{0}\" at line {1}: {2}", rawCommandText, line, reason));
    }
}
