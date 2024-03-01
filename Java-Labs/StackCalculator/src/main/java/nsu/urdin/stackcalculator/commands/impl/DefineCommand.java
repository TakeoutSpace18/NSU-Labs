package nsu.urdin.stackcalculator.commands.impl;

import nsu.urdin.stackcalculator.CalcContext;
import nsu.urdin.stackcalculator.commands.Command;
import nsu.urdin.stackcalculator.commands.CommandParser;
import nsu.urdin.stackcalculator.commands.exceptions.ArgumentsCountMismatchException;
import nsu.urdin.stackcalculator.commands.exceptions.CommandExecuteException;
import nsu.urdin.stackcalculator.commands.exceptions.WrongArgumentsFormatException;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;


public class DefineCommand extends Command {
    private static final Logger LOGGER = LogManager.getLogger(DefineCommand.class);
    private final String name;
    private final double value;

    public DefineCommand(CommandParser.Data commandData) {
        super(commandData.rawCommandText(), commandData.line());

        if (commandData.args().length < 2) {
            throw new ArgumentsCountMismatchException(getRawCommandText(), getLineNumber(), 2, commandData.args().length);
        }

        name = commandData.args()[0];
        if (!name.chars().allMatch(Character::isLetter)) {
            throw new WrongArgumentsFormatException(getRawCommandText(), getLineNumber(), "parameter name must contain only letter symbols");
        }

        try {
            value = Double.parseDouble(commandData.args()[1]);
        } catch (NumberFormatException e) {
            throw new WrongArgumentsFormatException(getRawCommandText(), getLineNumber(), "failed to parse floating point value");
        }
    }

    @Override
    public void exec(CalcContext context) throws CommandExecuteException {
        Double old = context.addParameter(name, value);
        if (old != null) {
            LOGGER.warn("Overwritten parameter {}: old value - {}, new - {}", name, old, value);
        }
        else {
            LOGGER.debug("Defined parameter {} with value {}", name, value);
        }
    }
}
