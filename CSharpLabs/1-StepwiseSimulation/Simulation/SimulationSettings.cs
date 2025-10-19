namespace Simulation;

public class SimulationSettings
{
    public string PhilosopherStrategy { set; get; } = "";
    public string PhilosopherCoordinator { set; get; } = "";
    public bool PrintStateChanges { set; get; } = false;
    public bool PreferCoordinator { set; get; } = false;

}