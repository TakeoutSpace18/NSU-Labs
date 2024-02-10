package commands;

import nsu.urdin.stackcalculator.CalcContext;
import nsu.urdin.stackcalculator.commands.Command;
import nsu.urdin.stackcalculator.commands.CommandParser;
import nsu.urdin.stackcalculator.commands.exceptions.CommandExecuteException;
import nsu.urdin.stackcalculator.commands.impl.DivideCommand;
import nsu.urdin.stackcalculator.commands.impl.MultiplyCommand;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;

public class DivideCommandTests {
    @Test
    void divideWorks() {
        CalcContext ctx = new CalcContext();
        ctx.pushValue(3);
        ctx.pushValue(15);

        Command divide = new DivideCommand(new CommandParser.Data());
        divide.exec(ctx);

        assertEquals(5, ctx.peekValue());
        assertEquals(1, ctx.stackSize());
    }

    @Test
    void throwsOnDivisionByZero() {
        CalcContext ctx = new CalcContext();
        ctx.pushValue(0);
        ctx.pushValue(15);

        Command divide = new DivideCommand(new CommandParser.Data());
        assertThrows(CommandExecuteException.class, () -> divide.exec(ctx));
    }
}
