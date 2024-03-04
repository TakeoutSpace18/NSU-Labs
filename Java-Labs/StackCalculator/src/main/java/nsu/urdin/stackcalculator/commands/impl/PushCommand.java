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
import org.apache.logging.log4j.spi.MessageFactory2Adapter;

import java.text.MessageFormat;

public class PushCommand extends Command {
    private static final Logger LOGGER = LogManager.getLogger(PushCommand.class);
    private double value;
    private final String parameterName;
    
    public PushCommand(CommandParser.Data commandData) {
        super(commandData.rawCommandText(), commandData.line());

        if (commandData.args().length < 1) {
            throw new ArgumentsCountMismatchException(getRawCommandText(), getLineNumber(), 1, commandData.args().length);
        }

        if (commandData.args()[0].chars().allMatch(Character::isLetter)) {
            parameterName = commandData.args()[0];
        }
        else {
            parameterName = null;
            try {
                value = Double.parseDouble(commandData.args()[0]);
            } catch (NumberFormatException e) {
                throw new WrongArgumentsFormatException(getRawCommandText(), getLineNumber(), "failed to parse floating point value");
            }
        }

    }
    
    @Override
    public void exec(CalcContext context) throws CommandExecuteException {
        if (parameterName != null) {
            try {
                value = context.getParameter(parameterName);
            } catch (UndefinedParameterException e) {
                throw new CommandExecuteException(getRawCommandText(), getLineNumber(), MessageFormat.format("Parameter \"{0}\" is undefined", parameterName));
            }
        }

        context.pushValue(value);
        LOGGER.debug("Pushed value {} on the stack", value);
    }
}
