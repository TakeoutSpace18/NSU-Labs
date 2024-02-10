package nsu.urdin.stackcalculator.commands;

import nsu.urdin.stackcalculator.commands.exceptions.CommandFileNotFoundException;
import nsu.urdin.stackcalculator.commands.exceptions.CommandParserException;

import java.io.*;
import java.nio.file.Path;
import java.util.Arrays;
import java.util.Optional;

public class CommandParser implements AutoCloseable {

    public record Data(String name, String[] args, String rawCommandText, int line) {
        public Data() {
            this("unnamed");
        }

        public Data(String name) {
            this(name, new String[0], null, 0);
        }

        @Override
        public boolean equals(Object obj) {
            if (this == obj) {
                return true;
            }

            if (obj == null || getClass() != obj.getClass()) {
                return false;
            }

            Data another = (Data) obj;
            return name.equals(another.name) &&
                    Arrays.equals(args, another.args) &&
                    rawCommandText.equals(another.rawCommandText) &&
                    line == another.line;
        }
    }

    private final BufferedReader cmdInput;
    private int currentLineNumber = 0;

    /**
     * Create CommandParser using default System.in input
     */
    public CommandParser() {
        cmdInput = new BufferedReader(new InputStreamReader(System.in));
    }

    /**
     * @param filePath path to the text file containing StackCalculator commands
     */
    public CommandParser(Path filePath) {
        try {
            cmdInput = new BufferedReader(new InputStreamReader(new FileInputStream(filePath.toFile())));
        } catch (FileNotFoundException e) {
            throw new CommandFileNotFoundException(filePath);
        }
    }

    public CommandParser(String source) {
        cmdInput = new BufferedReader(new StringReader(source));
    }

    /**
     * @return previously parsed command data (name and arguments).
     * @apiNote May return null if no command is available (because of EOF),
     * so the caller must check this situation by previously calling {@code hasNext() }
     */
    public Optional<Data> parseNext() {
        try {
            String line;
            while ((line = cmdInput.readLine()) != null) {
                currentLineNumber++;

                line = eraseComment(line);

                if (!line.isEmpty()) {
                    if (line.equalsIgnoreCase("exit")) {
                        return Optional.empty();
                    }

                    String[] tokens = line.split(" ");
                    return Optional.of(new Data(tokens[0], Arrays.copyOfRange(tokens, 1, tokens.length), line, currentLineNumber));
                }
            }
        } catch (IOException e) {
            throw new CommandParserException("Failed to parse next command!", e);
        }
        return Optional.empty();
    }

    private static String eraseComment(String line) {
        int commentStart = line.indexOf('#');
        if (commentStart != -1) {
            line = line.substring(0, commentStart).strip();
        }
        return line.strip();
    }

    @Override
    public void close() {
        try {
            cmdInput.close();
        } catch (IOException e) {
            throw new CommandParserException("Failed to close input stream!", e);
        }
    }
}
