package nsu.urdin.stackcalculator.commands;

import nsu.urdin.stackcalculator.CalcContext;

import java.util.List;

public interface Command {
    void apply(CalcContext context, String[] arguments);
}
