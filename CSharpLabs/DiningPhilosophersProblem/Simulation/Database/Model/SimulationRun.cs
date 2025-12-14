using System.ComponentModel.DataAnnotations;

namespace Simulation.Database.Model;

public class SimulationRun
{
    [Key]
    public Guid RunId { get; set; }
    
    public DateTime StartTime { get; set; }
    
    public DateTime? EndTime { get; set; }
    
    public ICollection<Philosopher> Philosophers { get; set; } = new List<Philosopher>();
    public ICollection<Fork> Forks { get; set; } = new List<Fork>();}