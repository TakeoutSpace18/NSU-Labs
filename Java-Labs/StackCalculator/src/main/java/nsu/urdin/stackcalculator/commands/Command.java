package nsu.urdin.stackcalculator.commands;

import nsu.urdin.stackcalculator.CalcContext;

public abstract class Command {
    private final String rawCommandText;
    private final int lineNumber;

    public Command(String rawCommandText, int lineNumber) {
        this.rawCommandText = rawCommandText;
        this.lineNumber = lineNumber;
    }

    protected String getRawCommandText() {
        return rawCommandText;
    }

    protected int getLineNumber() {
        return lineNumber;
    }

//    protected Logger getLogger() {
//        return Command.logger;
//    }

    public abstract void exec(CalcContext context);
}
