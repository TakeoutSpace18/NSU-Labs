namespace Simulation;

public class SimulationSettings
{
    public List<string> PhilosopherNames { get; set; } = [];
    public string PhilosopherStrategy { set; get; } = "";
    public string PhilosopherCoordinator { set; get; } = "";
    public bool DisplayStatus { set; get; }
    public bool PreferCoordinator { set; get; }
    public uint DurationSeconds { set; get; }
    public ActionTimes ActionTimes { set; get; } = new();
    public int StatusUpdateIntervalMs { set; get; }
    public int DeadlockCheckIntervalMs { set; get; }
}