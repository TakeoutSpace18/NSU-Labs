package commands.impl;

import nsu.urdin.stackcalculator.CalcContext;
import nsu.urdin.stackcalculator.commands.Command;
import nsu.urdin.stackcalculator.commands.CommandParser;
import nsu.urdin.stackcalculator.commands.exceptions.CommandExecuteException;
import nsu.urdin.stackcalculator.commands.impl.PushCommand;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;

public class PushCommandTests {
    @Test
    void worksWithExplicitValue() {
        CalcContext ctx = new CalcContext();

        Command push = new PushCommand(new CommandParser.Data(new String[]{"4.5"}));
        push.exec(ctx);

        assertEquals(4.5, ctx.peekValue());
        assertEquals(1, ctx.stackSize());
    }

    @Test
    void worksWithParameterValue() {
        CalcContext ctx = new CalcContext();
        ctx.addParameter("abc", 4.5);

        Command push = new PushCommand(new CommandParser.Data(new String[]{"abc"}));
        push.exec(ctx);

        assertEquals(4.5, ctx.peekValue());
        assertEquals(1, ctx.stackSize());
    }

    @Test
    void throwsOnNonexistentParameter() {
        CalcContext ctx = new CalcContext();

        Command push = new PushCommand(new CommandParser.Data(new String[]{"abc"}));
        assertThrows(CommandExecuteException.class, () -> push.exec(ctx));
    }


}
