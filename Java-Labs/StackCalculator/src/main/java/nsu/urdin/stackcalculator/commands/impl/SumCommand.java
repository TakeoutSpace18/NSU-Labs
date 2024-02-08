package nsu.urdin.stackcalculator.commands.impl;

import nsu.urdin.stackcalculator.CalcContext;
import nsu.urdin.stackcalculator.commands.Command;
import nsu.urdin.stackcalculator.commands.CommandParser;
import nsu.urdin.stackcalculator.commands.exceptions.CommandExecuteException;

import java.util.EmptyStackException;


public class SumCommand extends Command {

    public SumCommand(CommandParser.Data commandData) {
        super(commandData.rawCommandText(), commandData.line());
    }

    @Override
    public void exec(CalcContext context) {
        try {
            double o1 = context.getStack().pop();
            double o2 = context.getStack().pop();
            double result = o1 + o2;
            context.getStack().push(result);
            getLogger().debug("Executed Sum command: {} + {} = {}", o1, o2, result);
        } catch (EmptyStackException e) {
            throw new CommandExecuteException(getRawCommandText(), getLineNumber(), "Not enough operands on the stack");
        }

    }
}
