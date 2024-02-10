package nsu.urdin.stackcalculator.commands;

import nsu.urdin.stackcalculator.commands.exceptions.UnknownCommandException;
import nsu.urdin.stackcalculator.commands.exceptions.WrongArgumentsException;

import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.InvocationTargetException;
import java.util.Properties;

public class CommandFactory {
    public static String mappingsFilename = "/commandMappings.properties";

    private final Properties mappings;
    public CommandFactory() {
        mappings = new Properties();
        try (InputStream stream = CommandFactory.class.getResourceAsStream(mappingsFilename)) {
            mappings.load(stream);
        } catch (IOException e) {
            throw new RuntimeException("Failed to load command mappings!", e);
        }
    }

    public Command create(CommandParser.Data commandData) throws WrongArgumentsException {
        String cmdClassPath = mappings.getProperty(commandData.name());
        if (cmdClassPath == null) {
            throw new UnknownCommandException(commandData.name());
        }
        try {
            return (Command) Class.forName(cmdClassPath).getDeclaredConstructor(commandData.getClass()).newInstance(commandData);
        } catch (InstantiationException | ClassNotFoundException |
                 NoSuchMethodException | InvocationTargetException |
                 IllegalAccessException e) {
            throw new RuntimeException("Failed to create desired command class!", e);
        }
    }
}
