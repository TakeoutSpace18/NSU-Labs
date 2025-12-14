using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

namespace Simulation.Database.Model;

public class ForkStateRecord
{
    [Key] public long Key { get; set; }

    [Required] public int ForkKey { get; set; }

    [Required] [MaxLength(50)] public string State { get; set; } = string.Empty;

    public int? TakenByPhilosopherId { get; set; }

    [Required] public DateTime Timestamp { get; set; }

    [ForeignKey(nameof(ForkKey))] public Fork? Fork { get; set; }

    [ForeignKey(nameof(TakenByPhilosopherId))]
    public Philosopher? TakenByPhilosopher { get; set; }
}