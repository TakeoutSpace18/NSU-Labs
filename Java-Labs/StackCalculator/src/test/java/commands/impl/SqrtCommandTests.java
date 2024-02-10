package commands.impl;

import nsu.urdin.stackcalculator.CalcContext;
import nsu.urdin.stackcalculator.commands.Command;
import nsu.urdin.stackcalculator.commands.CommandParser;
import nsu.urdin.stackcalculator.commands.exceptions.CommandExecuteException;
import nsu.urdin.stackcalculator.commands.impl.SqrtCommand;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;

public class SqrtCommandTests {
    @Test
    void divideWorks() {
        CalcContext ctx = new CalcContext();
        ctx.pushValue(16);

        Command sqrt = new SqrtCommand(new CommandParser.Data());
        sqrt.exec(ctx);

        assertEquals(4, ctx.peekValue());
        assertEquals(1, ctx.stackSize());
    }

    @Test
    void throwsOnNegativeNumber() {
        CalcContext ctx = new CalcContext();
        ctx.pushValue(-16);

        Command sqrt = new SqrtCommand(new CommandParser.Data());
        assertThrows(CommandExecuteException.class, () -> sqrt.exec(ctx));
    }
}
