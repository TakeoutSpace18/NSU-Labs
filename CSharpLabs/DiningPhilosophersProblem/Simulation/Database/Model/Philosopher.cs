using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

namespace Simulation.Database.Model;

public class Philosopher
{
    [Key] public int Key { get; set; }
    [Required] public Guid RunId { get; set; }
    [Required] public uint Id { get; set; }
    [Required] [MaxLength(100)] public string Name { get; set; } = string.Empty;
    [ForeignKey(nameof(RunId))] public SimulationRun? SimulationRun { get; set; }
    public ICollection<PhilosopherStateRecord> States { get; set; } = new List<PhilosopherStateRecord>();
}