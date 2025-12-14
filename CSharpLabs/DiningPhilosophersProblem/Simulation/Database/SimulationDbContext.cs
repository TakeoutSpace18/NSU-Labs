using Microsoft.EntityFrameworkCore;
using Simulation.Database.Model;

namespace Simulation.Database;

public class SimulationDbContext(DbContextOptions<SimulationDbContext> options) : DbContext(options)
{
public DbSet<SimulationRun> SimulationRuns { get; set; }
    public DbSet<Philosopher> Philosophers { get; set; }
    public DbSet<Fork> Forks { get; set; }
    public DbSet<PhilosopherStateRecord> PhilosopherStates { get; set; }
    public DbSet<ForkStateRecord> ForkStates { get; set; }

    protected override void OnModelCreating(ModelBuilder modelBuilder)
    {
        base.OnModelCreating(modelBuilder);

        modelBuilder.Entity<SimulationRun>(entity =>
        {
            entity.HasKey(e => e.RunId);
            entity.HasIndex(e => e.StartTime);
        });

        modelBuilder.Entity<Philosopher>(entity =>
        {
            entity.HasKey(e => e.Key);
            entity.HasIndex(e => new { e.RunId, PhilosopherId = e.Id }).IsUnique();
            
            entity.HasOne(e => e.SimulationRun)
                .WithMany(r => r.Philosophers)
                .HasForeignKey(e => e.RunId)
                .OnDelete(DeleteBehavior.Cascade);
        });

        modelBuilder.Entity<Fork>(entity =>
        {
            entity.HasKey(e => e.Key);
            entity.HasIndex(e => new { e.RunId, e.Name }).IsUnique();
            
            entity.HasOne(e => e.SimulationRun)
                .WithMany(r => r.Forks)
                .HasForeignKey(e => e.RunId)
                .OnDelete(DeleteBehavior.Cascade);
        });

        modelBuilder.Entity<PhilosopherStateRecord>(entity =>
        {
            entity.HasKey(e => e.Key);
            entity.HasIndex(e => new { PhilosopherId = e.PhilosopherKey, e.Timestamp });
            
            entity.HasOne(e => e.Philosopher)
                .WithMany(p => p.States)
                .HasForeignKey(e => e.PhilosopherKey)
                .OnDelete(DeleteBehavior.Cascade);
        });

        modelBuilder.Entity<ForkStateRecord>(entity =>
        {
            entity.HasKey(e => e.Key);
            entity.HasIndex(e => new { ForkId = e.ForkKey, e.Timestamp });
            
            entity.HasOne(e => e.Fork)
                .WithMany(f => f.States)
                .HasForeignKey(e => e.ForkKey)
                .OnDelete(DeleteBehavior.Cascade);
            
            entity.HasOne(e => e.TakenByPhilosopher)
                .WithMany()
                .HasForeignKey(e => e.TakenByPhilosopherId)
                .OnDelete(DeleteBehavior.Restrict);
        });
    }}