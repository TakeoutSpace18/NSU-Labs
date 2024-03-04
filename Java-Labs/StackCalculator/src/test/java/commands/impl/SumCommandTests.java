package commands.impl;

import nsu.urdin.stackcalculator.CalcContext;
import nsu.urdin.stackcalculator.commands.Command;
import nsu.urdin.stackcalculator.commands.CommandParser;
import nsu.urdin.stackcalculator.commands.impl.SumCommand;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.*;

public class SumCommandTests {
    @Test
    void sumWorks() {
        CalcContext ctx = new CalcContext();
        ctx.pushValue(3);
        ctx.pushValue(5);

        Command sum = new SumCommand(new CommandParser.Data());
        sum.exec(ctx);

        assertEquals(8, ctx.peekValue());
        assertEquals(1, ctx.stackSize());
    }
}
