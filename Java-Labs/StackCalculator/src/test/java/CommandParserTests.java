import nsu.urdin.stackcalculator.commands.CommandParser;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;

import java.util.ArrayList;
import java.util.List;
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

                        List.of(new CommandParser.Data("1", new String[0]),
                                new CommandParser.Data("2", new String[0]),
                                new CommandParser.Data("3", new String[0]),
                                new CommandParser.Data("4", new String[0])
                        )),

                Arguments.of(
                        """
                                name a b c   
                                """,

                        List.of(new CommandParser.Data("name", new String[]{"a", "b", "c"})
                        )),

                Arguments.of(
                        """
                                # comment
                                1 # comment
                                    #comment
                                2 e #comment
                                #comment
                                """,

                        List.of(new CommandParser.Data("1", new String[0]),
                                new CommandParser.Data("2", new String[]{"e"})
                        )),

                Arguments.of("", List.of())
        );
    }

    @ParameterizedTest
    @MethodSource("testDataProvider")
    void test(String source, List<CommandParser.Data> expected) {
        CommandParser parser = new CommandParser(source);
        List<CommandParser.Data> result = new ArrayList<>();

        while (parser.hasNext()) {
            result.add(parser.getNext());
        }

        parser.close();

        assertIterableEquals(result, expected);
    }
}
