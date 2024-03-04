package commands.impl;

import nsu.urdin.stackcalculator.CalcContext;
import nsu.urdin.stackcalculator.commands.Command;
import nsu.urdin.stackcalculator.commands.CommandParser;
import nsu.urdin.stackcalculator.commands.impl.SubtractCommand;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;

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
}
