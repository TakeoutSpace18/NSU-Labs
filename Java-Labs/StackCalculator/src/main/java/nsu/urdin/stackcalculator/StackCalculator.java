package nsu.urdin.stackcalculator;

import nsu.urdin.stackcalculator.commands.Command;
import nsu.urdin.stackcalculator.commands.CommandFactory;
import nsu.urdin.stackcalculator.commands.CommandParser;
import nsu.urdin.stackcalculator.commands.exceptions.CommandExecuteException;
import nsu.urdin.stackcalculator.commands.impl.SumCommand;
import org.apache.commons.cli.*;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import javax.naming.Context;
import java.io.IOException;
import java.nio.file.Path;
import java.util.Optional;

public class StackCalculator {
    private static final Logger LOGGER = LogManager.getLogger(StackCalculator.class);
    private final CalcContext context;
    Option commandFileCLIOption;
    private final CommandLine parsedCLI;

    public StackCalculator(String[] args) {
        context = new CalcContext();

        commandFileCLIOption = Option.builder()
                .option("i")
                .longOpt("input")
                .hasArg()
                .argName("path")
                .desc("text file containing list of calculator commands to be executed")
                .build();

        Options cliOptions = new Options();
        cliOptions.addOption(commandFileCLIOption);

        CommandLineParser cliParser = new DefaultParser();
        try {
            parsedCLI = cliParser.parse(cliOptions, args);
        } catch (ParseException e) {
            throw new RuntimeException("Failed to parse CLI arguments!", e);
        }
    }

    public void run() throws IOException {
        CommandParser cmdParser;

        if (parsedCLI.hasOption(commandFileCLIOption)) {
            Path commandFilePath = Path.of(parsedCLI.getOptionValue(commandFileCLIOption));
            cmdParser = new CommandParser(commandFilePath);
            LOGGER.info("Starting Stack Calculator with file input: {}", commandFilePath);
        }
        else {
            cmdParser = new CommandParser();
            LOGGER.info("Starting Stack Calculator with console input");
        }

        CommandFactory factory = new CommandFactory();

        Optional<CommandParser.Data> commandData;
        while ((commandData = cmdParser.parseNext()).isPresent()) {
            Command command = factory.create(commandData.get());
            try {
                command.exec(context);
            } catch (CommandExecuteException e) {
                LOGGER.error(e.getMessage());
            }
        }

        cmdParser.close();
    }
}