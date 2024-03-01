package nsu.urdin.stackcalculator.commands.exceptions;

import nsu.urdin.stackcalculator.commands.exceptions.CommandParserException;

import java.nio.file.Path;
import java.text.MessageFormat;

public class CommandFileNotFoundException extends CommandParserException {
    public CommandFileNotFoundException(Path filePath) {
        super(MessageFormat.format("Couldn't find input command file \"{0}\"", filePath.toString()));
    }
}
