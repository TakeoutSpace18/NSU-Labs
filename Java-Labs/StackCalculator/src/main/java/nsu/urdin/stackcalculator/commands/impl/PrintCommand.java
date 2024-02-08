package nsu.urdin.stackcalculator.commands.impl;

import nsu.urdin.stackcalculator.CalcContext;
import nsu.urdin.stackcalculator.commands.Command;
import nsu.urdin.stackcalculator.commands.CommandParser;

import java.text.MessageFormat;


public class PrintCommand extends Command {

    public PrintCommand(CommandParser.Data commandData) {
        super(commandData.rawCommandText(), commandData.line());
    }

    @Override
    public void exec(CalcContext context) {
        if (context.getStack().isEmpty()) {
            context.getOutputCLIStream().println("Stack is empty, nothing to print!");
//            logger.debug("Stack top is empty, can't print");
        }
        else {
            context.getOutputCLIStream().println(MessageFormat.format("Top of the stack: {0}", context.getStack().peek()));
        }
    }
}
