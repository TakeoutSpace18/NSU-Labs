package nsu.urdin.stackcalculator.commands.impl;

import nsu.urdin.stackcalculator.CalcContext;
import nsu.urdin.stackcalculator.commands.Command;
import nsu.urdin.stackcalculator.commands.CommandParser;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.text.MessageFormat;


public class PrintCommand extends Command {

    private static final Logger LOGGER = LogManager.getLogger(PrintCommand.class);

    public PrintCommand(CommandParser.Data commandData) {
        super(commandData.rawCommandText(), commandData.line());
    }

    @Override
    public void exec(CalcContext context) {
        if (context.getStack().isEmpty()) {
            context.getOutputCLIStream().println("Stack is empty, nothing to print");
            LOGGER.debug("Stack is empty, nothing to print");
        }
        else {
            double value = context.getStack().peek();
            context.getOutputCLIStream().println(MessageFormat.format("Top of the stack: {0}", value));
            LOGGER.debug(MessageFormat.format("Printed top of the stack: {0}", value));
        }
    }
}
