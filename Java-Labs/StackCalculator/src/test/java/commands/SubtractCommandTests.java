package commands;

import nsu.urdin.stackcalculator.CalcContext;
import nsu.urdin.stackcalculator.commands.Command;
import nsu.urdin.stackcalculator.commands.CommandParser;
import nsu.urdin.stackcalculator.commands.exceptions.CommandExecuteException;
import nsu.urdin.stackcalculator.commands.impl.SubtractCommand;
import nsu.urdin.stackcalculator.commands.impl.SumCommand;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;

public class SubtractCommandTests {
    @Test
    void subtractWorks() {
        CalcContext ctx = new CalcContext();
        ctx.pushValue(3);
        ctx.pushValue(5);

        Command subtract = new SubtractCommand(new CommandParser.Data());
        subtract.exec(ctx);

        assertEquals(2, ctx.peekValue());
        assertEquals(1, ctx.stackSize());
    }

    @Test
    void throwsOnEmptyStack() {
        CalcContext ctx = new CalcContext();

        Command subtract = new SubtractCommand(new CommandParser.Data());

        assertThrows(CommandExecuteException.class, () -> subtract.exec(ctx));
    }
    @Test
    void throwsOnOneElementStack() {
        CalcContext ctx = new CalcContext();
        ctx.pushValue(2);

        Command subtract = new SubtractCommand(new CommandParser.Data());

        assertThrows(CommandExecuteException.class, () -> subtract.exec(ctx));
    }
}
