import nsu.urdin.stackcalculator.commands.Command;
import nsu.urdin.stackcalculator.commands.CommandParser;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;

import java.util.ArrayList;
import java.util.List;
import java.util.Optional;
import java.util.stream.Stream;

import static org.junit.jupiter.api.Assertions.assertIterableEquals;

public class CommandParserTests {
    static Stream<Arguments> testDataProvider() {
        return Stream.of(
                Arguments.of(
                        """
                                1
                                2
                                3
                                4
                                """,

                        List.of(new CommandParser.Data("1", new String[0], "1", 1),
                                new CommandParser.Data("2", new String[0], "2", 2),
                                new CommandParser.Data("3", new String[0], "3", 3),
                                new CommandParser.Data("4", new String[0], "4", 4)
                        )),

                Arguments.of(
                        """
                                name a b c   
                                """,

                        List.of(new CommandParser.Data("name", new String[]{"a", "b", "c"}, "name a b c", 1)
                        )),

                Arguments.of(
                        """
                                # comment
                                1 # comment
                                    #comment
                                2 e #comment
                                #comment
                                """,

                        List.of(new CommandParser.Data("1", new String[0], "1", 2),
                                new CommandParser.Data("2", new String[]{"e"}, "2 e", 4)
                        )),

                Arguments.of("", List.of())
        );
    }

    @ParameterizedTest
    @MethodSource("testDataProvider")
    void test(String source, List<CommandParser.Data> expected) {
        CommandParser parser = new CommandParser(source);
        List<CommandParser.Data> result = new ArrayList<>();

        Optional<CommandParser.Data> commandData;
        while ((commandData = parser.parseNext()).isPresent()) {
            result.add(commandData.get());
        }

        parser.close();

        assertIterableEquals(result, expected);
    }
}
