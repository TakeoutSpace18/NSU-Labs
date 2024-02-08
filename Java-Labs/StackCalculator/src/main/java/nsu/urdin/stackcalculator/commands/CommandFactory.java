package nsu.urdin.stackcalculator.commands;

import nsu.urdin.stackcalculator.commands.exceptions.UnknownCommandException;

import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.InvocationTargetException;
import java.util.Properties;

public class CommandFactory {
    public static String mappingsFilename = "/commandMappings.properties";

    private final Properties mappings;
    public CommandFactory(){
        mappings = new Properties();
        try (InputStream stream = CommandFactory.class.getResourceAsStream(mappingsFilename)) {
            mappings.load(stream);
        } catch (IOException e) {
            throw new RuntimeException("Failed to load command mappings!", e);
        }
    }

    public Command create(String commandName) {
        String cmdClassPath = mappings.getProperty(commandName);
        if (cmdClassPath == null) {
            throw new UnknownCommandException(commandName);
        }
        try {
            return (Command) Class.forName(cmdClassPath).getDeclaredConstructor().newInstance();
        } catch (InstantiationException | ClassNotFoundException | NoSuchMethodException | InvocationTargetException |
                 IllegalAccessException e) {
            throw new RuntimeException("Failed to create desired command class!", e);
        }
    }
}
