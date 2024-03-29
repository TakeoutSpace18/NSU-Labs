package nsu.urdin.stackcalculator.commands.impl;

import nsu.urdin.stackcalculator.CalcContext;
import nsu.urdin.stackcalculator.commands.Command;
import nsu.urdin.stackcalculator.commands.CommandParser;
import nsu.urdin.stackcalculator.commands.exceptions.CommandExecuteException;
import nsu.urdin.stackcalculator.commands.exceptions.NotEnoughOperandsException;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

public class SubtractCommand extends Command {
    private static final Logger LOGGER = LogManager.getLogger(SubtractCommand.class);

    public SubtractCommand(CommandParser.Data commandData) {
        super(commandData.rawCommandText(), commandData.line());
    }

    @Override
    public void exec(CalcContext context) throws CommandExecuteException {
        if (context.stackSize() < 2) {
            throw new NotEnoughOperandsException(getRawCommandText(), getLineNumber());
        }

        double o1 = context.popValue();
        double o2 = context.popValue();
        double result = o1 - o2;
        context.pushValue(result);
        LOGGER.debug("Executed subtract command: {} - {} = {}", o1, o2, result);
    }
}
