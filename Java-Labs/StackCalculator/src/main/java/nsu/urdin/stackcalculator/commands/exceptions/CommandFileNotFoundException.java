package nsu.urdin.stackcalculator.commands.exceptions;

import nsu.urdin.stackcalculator.commands.exceptions.CommandParserException;

import java.nio.file.Path;

public class CommandFileNotFoundException extends CommandParserException {
    public CommandFileNotFoundException(Path filePath) {
        super(String.format("Couldn't find command filePath \'%s\'", filePath.toString()));
    }
}
