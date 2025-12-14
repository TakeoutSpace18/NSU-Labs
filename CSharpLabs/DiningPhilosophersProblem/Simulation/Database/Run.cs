namespace Simulation.Database;

public class Run
{
    public long Id { get; set; }
    public Guid RunGuid { get; set; }
    public DateTimeOffset StartedAtUtc { get; set; }
    public string? Description { get; set; }
}