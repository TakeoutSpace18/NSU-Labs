namespace Simulation;

public class ActionTimes
{
    public int ThinkingMinMs { set; get; } = 0;
    public int ThinkingMaxMs { set; get; } = 0;
    public int EatingMinMs { set; get; } = 0;
    public int EatingMaxMs { set; get; } = 0;
    public int ForkAcquisitionMs  { set; get; } = 0;
}