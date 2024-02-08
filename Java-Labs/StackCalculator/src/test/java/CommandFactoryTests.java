import nsu.urdin.stackcalculator.commands.Command;
import nsu.urdin.stackcalculator.commands.CommandFactory;
import nsu.urdin.stackcalculator.commands.CommandParser;
import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;

import java.io.IOException;
import java.util.Properties;

public class CommandFactoryTests
{
    private final CommandFactory factory;
    private final Properties mappings;

    public CommandFactoryTests() throws IOException {
        this.factory = new CommandFactory();
        this.mappings = new Properties();
        this.mappings.load(CommandFactory.class.getResourceAsStream(CommandFactory.mappingsFilename));
    }

    @Test
    void createsCorrectObjectForAllMappings() throws ClassNotFoundException {
        for (String name : mappings.stringPropertyNames()) {
            CommandParser.Data commandData = new CommandParser.Data(name);
            Command cmd = factory.create(commandData);
            Assertions.assertInstanceOf(Class.forName(mappings.getProperty(name)), cmd);
        }
    }
}
