using System.Collections.Concurrent;
using System.Text.Json;
using Microsoft.EntityFrameworkCore;
using Simulation.Database.Model;

namespace Simulation.Database;

public class SimulationDbManager : IDisposable
{
    private readonly DbContextOptions<SimulationDbContext> _options;
    private readonly ConcurrentDictionary<uint, int> _philosopherId2KeyMap = new();
    private readonly ConcurrentDictionary<uint, int> _forkId2KeyMap = new();
    private readonly SemaphoreSlim _semaphore = new(1, 1);

    public Guid RunId { get; }
    public DateTime StartTime { get; }

    public SimulationDbManager(DbContextOptions<SimulationDbContext> options)
    {
        _options = options;
        RunId = Guid.NewGuid();
        StartTime = DateTime.UtcNow;

        // Create record about new run
        using var context = new SimulationDbContext(_options);
        var run = new SimulationRun
        {
            RunId = RunId,
            StartTime = StartTime
        };
        context.SimulationRuns.Add(run);
        context.SaveChanges();
        
        Console.WriteLine($"Simulation RunId: {RunId}");

    }

    public async Task<int> RegisterPhilosopherAsync(uint philosopherId, string philosopherName)
    {
        if (_philosopherId2KeyMap.TryGetValue(philosopherId, out var dbId))
            return dbId;

        await _semaphore.WaitAsync();
        try
        {
            // Double-check after acquiring lock
            if (_philosopherId2KeyMap.TryGetValue(philosopherId, out dbId))
                return dbId;

            using var context = new SimulationDbContext(_options);

            var philosopher = new Philosopher
            {
                RunId = RunId,
                Id = philosopherId,
                Name = philosopherName
            };

            await context.Philosophers.AddAsync(philosopher);
            await context.SaveChangesAsync();

            _philosopherId2KeyMap[philosopherId] = philosopher.Key;
            return philosopher.Key;
        }
        finally
        {
            _semaphore.Release();
        }
    }

    public async Task<int> RegisterForkAsync(uint forkId, string forkName)
    {
        if (_forkId2KeyMap.TryGetValue(forkId, out var dbId))
            return dbId;

        await _semaphore.WaitAsync();
        try
        {
            // Double-check after acquiring lock
            if (_forkId2KeyMap.TryGetValue(forkId, out dbId))
                return dbId;

            using var context = new SimulationDbContext(_options);

            var fork = new Fork
            {
                RunId = RunId,
                Name = forkName
            };

            await context.Forks.AddAsync(fork);
            await context.SaveChangesAsync();

            _forkId2KeyMap[forkId] = fork.Key;
            return fork.Key;
        }
        finally
        {
            _semaphore.Release();
        }
    }

    public async Task LogPhilosopherStateAsync(uint philosopherId, string state, long eaten, DateTime timestamp)
    {
        if (!_philosopherId2KeyMap.TryGetValue(philosopherId, out var philosopherDbKey))
        {
            throw new KeyNotFoundException($"Philosopher with id {philosopherId} was not registered");
        }

        using var context = new SimulationDbContext(_options);

        var record = new PhilosopherStateRecord
        {
            PhilosopherKey = philosopherDbKey,
            State = state,
            Eaten = eaten,
            Timestamp = timestamp
        };

        await context.PhilosopherStates.AddAsync(record);
        await context.SaveChangesAsync();
    }

    public async Task LogForkStateAsync(uint forkId, string state, uint? takenByPhilosopherId, DateTime timestamp)
    {
        if (!_forkId2KeyMap.TryGetValue(forkId, out var forkDbKey))
        {
            throw new KeyNotFoundException($"Fork with id {forkId} was not registered");
        }

        int? takenByPhilosopherDbKey = null;
        if (takenByPhilosopherId != null)
        {
            if (!_philosopherId2KeyMap.TryGetValue((uint)takenByPhilosopherId, out var dbKey))
            {
                throw new KeyNotFoundException($"Philosopher with id {takenByPhilosopherId} was not registered");
            }
            takenByPhilosopherDbKey = dbKey;
        }

        using var context = new SimulationDbContext(_options);

        var record = new ForkStateRecord
        {
            ForkKey = forkDbKey,
            State = state,
            TakenByPhilosopherId = takenByPhilosopherDbKey,
            Timestamp = timestamp
        };

        await context.ForkStates.AddAsync(record);
        await context.SaveChangesAsync();
    }

    public async Task<SimulationSnapshot?> GetSnapshotAsync(Guid runId, double delaySeconds)
    {
        await using var context = new SimulationDbContext(_options);

        var run = await context.SimulationRuns
            .FirstOrDefaultAsync(r => r.RunId == runId);

        if (run == null)
            return null;

        var targetTime = run.StartTime.AddSeconds(delaySeconds);

        var philosophers = await context.Philosophers
            .Where(p => p.RunId == runId)
            .ToListAsync();

        var philosopherStates = new List<PhilosopherStateRecord>();
        foreach (var philosopher in philosophers)
        {
            var latestState = await context.PhilosopherStates
                .Where(ps => ps.PhilosopherKey == philosopher.Key && ps.Timestamp <= targetTime)
                .OrderByDescending(ps => ps.Timestamp)
                .Include(ps => ps.Philosopher)
                .FirstOrDefaultAsync();

            if (latestState != null)
                philosopherStates.Add(latestState);
        }

        var forks = await context.Forks
            .Where(f => f.RunId == runId)
            .ToListAsync();

        var forkStates = new List<ForkStateRecord>();
        foreach (var fork in forks)
        {
            var latestState = await context.ForkStates
                .Where(fs => fs.ForkKey == fork.Key && fs.Timestamp <= targetTime)
                .OrderByDescending(fs => fs.Timestamp)
                .Include(fs => fs.Fork)
                .Include(fs => fs.TakenByPhilosopher)
                .FirstOrDefaultAsync();

            if (latestState != null)
                forkStates.Add(latestState);
        }

        return new SimulationSnapshot
        {
            RunId = runId,
            StartTime = run.StartTime,
            SnapshotTime = targetTime,
            PhilosopherStates = philosopherStates,
            ForkStates = forkStates
        };
    }

    public void Dispose()
    {
        _semaphore.Dispose();
    }
}