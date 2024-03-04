package commands;

import nsu.urdin.stackcalculator.CalcContext;
import nsu.urdin.stackcalculator.commands.Command;
import nsu.urdin.stackcalculator.commands.CommandParser;
import nsu.urdin.stackcalculator.commands.exceptions.CommandExecuteException;
import nsu.urdin.stackcalculator.commands.impl.*;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.MethodSource;

import java.util.stream.Stream;

import static org.junit.jupiter.api.Assertions.assertThrows;

public class CommonCommandTests {

    static Stream<Command> commandsRequiringTwoArgumentsProvider() {
        return Stream.of(
                new SubtractCommand(new CommandParser.Data()),
                new SumCommand(new CommandParser.Data()),
                new MultiplyCommand(new CommandParser.Data()),
                new DivideCommand(new CommandParser.Data())
        );
    }

    static Stream<Command> commandsRequiringOneArgumentProvider() {
        return Stream.of(
                new PopCommand(new CommandParser.Data()),
                new SqrtCommand(new CommandParser.Data())
        );
    }

    @ParameterizedTest
    @MethodSource({"commandsRequiringOneArgumentProvider", "commandsRequiringTwoArgumentsProvider"})
    void throwsOnEmptyStack(Command cmd) {
        CalcContext ctx = new CalcContext();

        assertThrows(CommandExecuteException.class, () -> cmd.exec(ctx));
    }
    @ParameterizedTest
    @MethodSource({"commandsRequiringTwoArgumentsProvider"})
    void throwsOnOneElementStack(Command cmd) {
        CalcContext ctx = new CalcContext();
        ctx.pushValue(2);

        assertThrows(CommandExecuteException.class, () -> cmd.exec(ctx));
    }
}
