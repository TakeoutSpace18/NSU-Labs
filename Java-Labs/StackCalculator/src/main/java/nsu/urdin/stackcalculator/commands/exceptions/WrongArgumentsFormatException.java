package nsu.urdin.stackcalculator.commands.exceptions;

import java.text.MessageFormat;

public class WrongArgumentsFormatException extends WrongArgumentsException {
    public WrongArgumentsFormatException(String rawCommandText, int line, String reason) {
        super(MessageFormat.format("Invalid arguments format in command \"{0}\" at line {1}: {2}", rawCommandText, line, reason));
    }
}
