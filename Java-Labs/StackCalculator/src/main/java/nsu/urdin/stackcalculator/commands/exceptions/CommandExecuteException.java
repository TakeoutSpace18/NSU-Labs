package nsu.urdin.stackcalculator.commands.exceptions;

import nsu.urdin.stackcalculator.exceptions.StackCalculatorException;

import java.text.MessageFormat;

public class CommandExecuteException extends StackCalculatorException {
    public CommandExecuteException(String rawCommandText, int line, String reason) {
        super(MessageFormat.format("Failed to execute command \"{0}\" at line {1}: {2}", rawCommandText, line, reason));
    }
}
