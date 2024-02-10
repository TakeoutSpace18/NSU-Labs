import nsu.urdin.stackcalculator.CalcContext;
import nsu.urdin.stackcalculator.commands.Command;
import nsu.urdin.stackcalculator.commands.CommandParser;
import nsu.urdin.stackcalculator.commands.exceptions.WrongArgumentsFormatException;
import nsu.urdin.stackcalculator.commands.impl.DefineCommand;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;

public class DefineCommandTests {
    @Test
    void defineWorks() {
        CalcContext ctx = new CalcContext();

        Command define = new DefineCommand(new CommandParser.Data(new String[]{"abcd", "1.5"}));
        define.exec(ctx);

        assertEquals(1.5, ctx.getParameter("abcd"));
    }

    @Test
    void throwsOnBadName() {
        assertThrows(WrongArgumentsFormatException.class, () -> new DefineCommand(new CommandParser.Data(new String[]{"a234bcd", "1.5"})));
    }
    @Test
    void throwsOnBadValue() {
        assertThrows(WrongArgumentsFormatException.class, () -> new DefineCommand(new CommandParser.Data(new String[]{"abcd", "1.g5"})));
    }
    @Test
    void throwsOnNoArgs() {
        assertThrows(WrongArgumentsFormatException.class, () -> new DefineCommand(new CommandParser.Data(new String[0])));
    }
}
