package nsu.urdin.stackcalculator.commands.impl;

import nsu.urdin.stackcalculator.CalcContext;
import nsu.urdin.stackcalculator.commands.Command;
import nsu.urdin.stackcalculator.commands.CommandParser;
import nsu.urdin.stackcalculator.commands.exceptions.ArgumentsCountMismatchException;
import nsu.urdin.stackcalculator.commands.exceptions.CommandExecuteException;
import nsu.urdin.stackcalculator.commands.exceptions.WrongArgumentsFormatException;
import nsu.urdin.stackcalculator.exceptions.UndefinedParameterException;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.text.MessageFormat;

public class PopCommand extends Command {
    private static final Logger LOGGER = LogManager.getLogger(PushCommand.class);

    public PopCommand(CommandParser.Data commandData) {
        super(commandData.rawCommandText(), commandData.line());
    }

    @Override
    public void exec(CalcContext context) throws CommandExecuteException {
        if (context.isStackEmpty()) {
            throw new CommandExecuteException(getRawCommandText(), getLineNumber(), "stack is empty");
        }

        double val = context.popValue();
        LOGGER.debug("Popped value {} from stack", val);
    }
}
