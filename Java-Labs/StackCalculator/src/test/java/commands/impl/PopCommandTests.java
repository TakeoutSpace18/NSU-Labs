package commands.impl;

import nsu.urdin.stackcalculator.CalcContext;
import nsu.urdin.stackcalculator.commands.Command;
import nsu.urdin.stackcalculator.commands.CommandParser;
import nsu.urdin.stackcalculator.commands.exceptions.CommandExecuteException;
import nsu.urdin.stackcalculator.commands.impl.PopCommand;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

public class PopCommandTests {
    @Test
    void popWorks() {
        CalcContext ctx = new CalcContext();
        ctx.pushValue(3);

        Command pop = new PopCommand(new CommandParser.Data());
        pop.exec(ctx);

        assertTrue(ctx.isStackEmpty());
    }

    @Test
    void throwsOnEmptyStack() {
        CalcContext ctx = new CalcContext();

        Command pop = new PopCommand(new CommandParser.Data());
        assertThrows(CommandExecuteException.class, () -> pop.exec(ctx));
    }
}
