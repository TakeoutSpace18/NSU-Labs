namespace Simulation;

public class ActionTimes
{
    public int ThinkingMinMs { init; get; } = 0;
    public int ThinkingMaxMs { init; get; } = 0;
    public int EatingMinMs { init; get; } = 0;
    public int EatingMaxMs { init; get; } = 0;
    public int ForkAcquisitionMs  { init; get; } = 0;
}