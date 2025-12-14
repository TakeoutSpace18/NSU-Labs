using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

namespace Simulation.Database.Model;

public class Fork
{
    [Key] public int Key { get; set; }
    [Required] public Guid RunId { get; set; }
    [Required] [MaxLength(100)] public string Name { get; set; } = string.Empty;
    [Required] public uint ForkId { get; set; }
    [ForeignKey(nameof(RunId))] public SimulationRun? SimulationRun { get; set; }
    public ICollection<ForkStateRecord> States { get; set; } = new List<ForkStateRecord>();
}