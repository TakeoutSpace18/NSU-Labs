package nsu.urdin.stackcalculator;

import nsu.urdin.stackcalculator.commands.Command;
import nsu.urdin.stackcalculator.commands.CommandFactory;
import nsu.urdin.stackcalculator.commands.CommandParser;
import org.apache.commons.cli.*;

import java.nio.file.Path;

public class StackCalculator {
    private final CalcContext context;
    Option commandFileCLIOption;
    private final CommandLine parsedCLI;

    public StackCalculator(String[] args) {
        context = new CalcContext();

        commandFileCLIOption = new Option("input",
                true,
                "Path to the file containing list of calculator commands to be executed");
        Options cliOptions = new Options();
        cliOptions.addOption(commandFileCLIOption);

        CommandLineParser cliParser = new DefaultParser();
        try {
            parsedCLI = cliParser.parse(cliOptions, args);
        } catch (ParseException e) {
            throw new RuntimeException("Failed to parse CLI arguments!", e);
        }
    }

    public void run() {
        CommandParser cmdParser;

        if (parsedCLI.hasOption(commandFileCLIOption)) {
            Path commandFilePath = Path.of(parsedCLI.getOptionValue(commandFileCLIOption));
            cmdParser = new CommandParser(commandFilePath);
        }
        else {
            cmdParser = new CommandParser();
        }

        CommandFactory factory = new CommandFactory();

        while (cmdParser.hasNext()) {
            CommandParser.Data commandData = cmdParser.getNext();
            Command command = factory.create(commandData.name());
            command.apply(context, commandData.args());
        }

        cmdParser.close();
    }
}