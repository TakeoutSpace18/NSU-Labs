package nsu.urdin.stackcalculator.commands;

import nsu.urdin.stackcalculator.exceptions.CommandFileNotFoundException;
import nsu.urdin.stackcalculator.exceptions.CommandParserException;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.nio.file.Path;
import java.util.Arrays;
import java.util.Scanner;

public class CommandParser implements AutoCloseable {

    public record Data(String name, String[] args) {
        @Override
        public boolean equals(Object another1) {
            Data another = (Data)another1;
            return name.equals(another.name) && Arrays.equals(args, another.args);
        }
    }

    private final Scanner cmdInput;
    private Data currentData;

    /**
     * Create CommandParser using default System.in input
     */
    public CommandParser() {
        cmdInput = new Scanner(System.in);
        parseNext();
    }

    /**
     * @param filePath path to the text file containing StackCalculator commands
     */
    public CommandParser(Path filePath) {
        try {
            cmdInput = new Scanner(filePath);
        } catch (FileNotFoundException e) {
            throw new CommandFileNotFoundException(filePath);
        }
        catch (IOException e) {
            throw new CommandParserException(e.getLocalizedMessage());
        }

        parseNext();
    }

    public CommandParser(String source) {
        cmdInput = new Scanner(source);
        parseNext();
    }


    /**
     * Check if a command is available for reading
     */
    public boolean hasNext() {
        return currentData != null;
    }

    /**
     * @return previously parsed command data (name and arguments).
     * @apiNote May return null if no command is available (because of EOF),
     * so the caller must check this situation by previously calling {@code hasNext() }
     */
    public Data getNext() {
        Data ret = currentData;
        parseNext();
        return ret;
    }

    private void parseNext() {
        String line = "";

        while (cmdInput.hasNextLine() && line.isBlank()) {
            line = cmdInput.nextLine();
            int commentStart = line.indexOf('#');
            if (commentStart != -1) {
                line = line.substring(0, commentStart);
            }
        }

        if (!line.isBlank()) {
            String[] tokens = line.split(" ");
            currentData = new Data(tokens[0], Arrays.copyOfRange(tokens, 1, tokens.length));
        }
        else {
            currentData = null;
        }
    }

    @Override
    public void close() {
        cmdInput.close();
    }


}
