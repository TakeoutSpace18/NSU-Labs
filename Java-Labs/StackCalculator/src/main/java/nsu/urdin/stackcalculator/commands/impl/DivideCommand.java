package nsu.urdin.stackcalculator.commands.impl;

import nsu.urdin.stackcalculator.CalcContext;
import nsu.urdin.stackcalculator.commands.Command;
import nsu.urdin.stackcalculator.commands.CommandParser;
import nsu.urdin.stackcalculator.commands.exceptions.CommandExecuteException;
import nsu.urdin.stackcalculator.commands.exceptions.NotEnoughOperandsException;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

public class DivideCommand extends Command {
    private static final Logger LOGGER = LogManager.getLogger(DivideCommand.class);

    public DivideCommand(CommandParser.Data commandData) {
        super(commandData.rawCommandText(), commandData.line());
    }

    @Override
    public void exec(CalcContext context) throws CommandExecuteException {
        if (context.stackSize() < 2) {
            throw new NotEnoughOperandsException(getRawCommandText(), getLineNumber());
        }

        double o1 = context.popValue();
        double o2 = context.popValue();

        if (o2 == 0) {
            context.pushValue(o2);
            context.pushValue(o1);
            throw new CommandExecuteException(getRawCommandText(), getLineNumber(), "division by zero");
        }

        double result = o1 / o2;
        context.pushValue(result);
        LOGGER.debug("Executed divide command: {} / {} = {}", o1, o2, result);
    }
}
