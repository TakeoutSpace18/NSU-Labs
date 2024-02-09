package nsu.urdin.stackcalculator.commands.impl;

import nsu.urdin.stackcalculator.CalcContext;
import nsu.urdin.stackcalculator.commands.Command;
import nsu.urdin.stackcalculator.commands.CommandParser;
import nsu.urdin.stackcalculator.commands.exceptions.CommandArgumentsFormatException;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

public class PushCommand extends Command {
    private static final Logger LOGGER = LogManager.getLogger(PushCommand.class);
    private final double value;
    
    public PushCommand(CommandParser.Data commandData) {
        super(commandData.rawCommandText(), commandData.line());
        if (commandData.args().length < 1) {
            throw new CommandArgumentsFormatException(getRawCommandText(), getLineNumber(), "no arguments specified, expected 1");
        }

        try {
            value = Double.parseDouble(commandData.args()[0]);
        } catch (NumberFormatException e) {
            throw new CommandArgumentsFormatException(getRawCommandText(), getLineNumber(), "failed to parse floating point value");
        }
    }
    
    @Override
    public void exec(CalcContext context) {
        context.getStack().push(value);
        LOGGER.debug("Pushed value {} on the stack", value);
    }
}
