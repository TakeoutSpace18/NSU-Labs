using Simulation.Database.Model;

namespace Simulation.Database;

public class SimulationSnapshot
{
    public Guid RunId { get; set; }
    public DateTime StartTime { get; set; }
    public DateTime SnapshotTime { get; set; }
    public List<PhilosopherStateRecord> PhilosopherStates { get; set; } = [];
    public List<ForkStateRecord> ForkStates { get; set; } = [];
}