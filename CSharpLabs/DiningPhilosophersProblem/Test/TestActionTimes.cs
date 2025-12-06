using Simulation;

namespace Test;

public static class TestActionTimes
{
    public static ActionTimes Fast => new ActionTimes
    {
        ThinkingMinMs = 1,
        ThinkingMaxMs = 1,
        EatingMinMs = 1,
        EatingMaxMs = 1,
        ForkAcquisitionMs = 1
    };
}