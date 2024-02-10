package nsu.urdin.stackcalculator.commands.exceptions;

import java.text.MessageFormat;

public class ArgumentsCountMismatchException extends WrongArgumentsException {
    public ArgumentsCountMismatchException(String rawCommandText, int line, int expected, int actual) {
        super(MessageFormat.format("Wrong arguments count in command \"{0}\" at line {1}: expected {2}, got {3}", rawCommandText, line, expected, actual));
    }
}
