package nsu.urdin.stackcalculator.commands.impl;

import nsu.urdin.stackcalculator.CalcContext;
import nsu.urdin.stackcalculator.commands.Command;
import nsu.urdin.stackcalculator.commands.CommandParser;
import nsu.urdin.stackcalculator.commands.exceptions.CommandExecuteException;
import nsu.urdin.stackcalculator.commands.exceptions.NotEnoughOperandsException;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

public class SqrtCommand extends Command {
    private static final Logger LOGGER = LogManager.getLogger(SqrtCommand.class);

    public SqrtCommand(CommandParser.Data commandData) {
        super(commandData.rawCommandText(), commandData.line());
    }

    @Override
    public void exec(CalcContext context) throws CommandExecuteException {
        if (context.stackSize() < 1) {
            throw new NotEnoughOperandsException(getRawCommandText(), getLineNumber());
        }

        double o1 = context.popValue();
        if (o1 < 0) {
            throw new CommandExecuteException(getRawCommandText(), getLineNumber(), "taking square root of negative value is impossible");
        }

        double result = Math.sqrt(o1);
        context.pushValue(result);
        LOGGER.debug("Executed sqrt command: sqrt({})= {}", o1, result);
    }
}
