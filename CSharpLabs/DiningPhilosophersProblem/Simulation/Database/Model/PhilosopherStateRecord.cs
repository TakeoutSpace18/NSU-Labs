using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

namespace Simulation.Database.Model;

public class PhilosopherStateRecord
{
    [Key] public long Key { get; set; }

    [Required] public int PhilosopherKey { get; set; }

    [Required] [MaxLength(50)] public string State { get; set; } = string.Empty;

    public long Eaten { get; set; }

    [Required] public DateTime Timestamp { get; set; }

    [ForeignKey(nameof(PhilosopherKey))] public Philosopher? Philosopher { get; set; }
}