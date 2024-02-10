package commands;

import nsu.urdin.stackcalculator.CalcContext;
import nsu.urdin.stackcalculator.commands.Command;
import nsu.urdin.stackcalculator.commands.CommandParser;
import nsu.urdin.stackcalculator.commands.impl.MultiplyCommand;
import nsu.urdin.stackcalculator.commands.impl.SumCommand;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;

public class MultiplyCommandTests {
    @Test
    void multiplyWorks() {
        CalcContext ctx = new CalcContext();
        ctx.pushValue(3);
        ctx.pushValue(5);

        Command multiply = new MultiplyCommand(new CommandParser.Data());
        multiply.exec(ctx);

        assertEquals(15, ctx.peekValue());
        assertEquals(1, ctx.stackSize());
    }
}
