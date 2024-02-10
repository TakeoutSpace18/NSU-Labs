package nsu.urdin.stackcalculator.commands.impl;

import nsu.urdin.stackcalculator.CalcContext;
import nsu.urdin.stackcalculator.commands.Command;
import nsu.urdin.stackcalculator.commands.CommandParser;
import nsu.urdin.stackcalculator.commands.exceptions.CommandExecuteException;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.util.EmptyStackException;


public class SumCommand extends Command {
    private static final Logger LOGGER = LogManager.getLogger(SumCommand.class);

    public SumCommand(CommandParser.Data commandData) {
        super(commandData.rawCommandText(), commandData.line());
    }

    @Override
    public void exec(CalcContext context) {
        if (context.getStack().size() < 2) {
            throw new CommandExecuteException(getRawCommandText(), getLineNumber(), "Not enough operands on the stack");
        }

        double o1 = context.getStack().pop();
        double o2 = context.getStack().pop();
        double result = o1 + o2;
        context.getStack().push(result);
        LOGGER.debug("Executed Sum command: {} + {} = {}", o1, o2, result);
    }
}
