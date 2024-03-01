package nsu.urdin.stackcalculator.commands.exceptions;

import java.text.MessageFormat;

public class UnknownCommandException extends RuntimeException {
    public UnknownCommandException(String commandName) {
        super(MessageFormat.format("Unknown command \"{0}\"!", commandName));
    }
}
