using Microsoft.Data.Sqlite;
using Microsoft.EntityFrameworkCore;
using Simulation.Database;
using Xunit;

namespace Test;

public class SimulationDbManagerTests : IDisposable
{
    private readonly SqliteConnection _connection;
    private readonly DbContextOptions<SimulationDbContext> _options;
    private readonly SimulationDbManager _dbManager;

    public SimulationDbManagerTests()
    {
        _connection = new SqliteConnection("DataSource=:memory:");
        _connection.Open();

        _options = new DbContextOptionsBuilder<SimulationDbContext>()
            .UseSqlite(_connection)
            .Options;

        using (var context = new SimulationDbContext(_options))
        {
            context.Database.EnsureCreated();
        }

        _dbManager = new SimulationDbManager(_options);
    }

    public void Dispose()
    {
        _dbManager.Dispose();
        _connection.Close();
        _connection.Dispose();
    }

    [Fact]
    public void Constructor_CreatesSimulationRun()
    {
        using var context = new SimulationDbContext(_options);
        var run = context.SimulationRuns.FirstOrDefault(r => r.RunId == _dbManager.RunId);

        Assert.NotNull(run);
        Assert.Equal(_dbManager.RunId, run.RunId);
        Assert.Equal(_dbManager.StartTime, run.StartTime);
    }

    [Fact]
    public async Task RegisterPhilosopherAsync_RegistersNewPhilosopher()
    {
        const uint philosopherId = 1;
        const string philosopherName = "Socrates";

        var dbKey = await _dbManager.RegisterPhilosopherAsync(philosopherId, philosopherName);

        Assert.True(dbKey > 0);

        var context = new SimulationDbContext(_options);
        var philosopher = await context.Philosophers.FirstOrDefaultAsync(p => p.Key == dbKey);
        
        Assert.NotNull(philosopher);
        Assert.Equal(philosopherId, philosopher.Id);
        Assert.Equal(philosopherName, philosopher.Name);
        Assert.Equal(_dbManager.RunId, philosopher.RunId);
    }

    [Fact]
    public async Task RegisterPhilosopherAsync_ReturnsSameKeyForDuplicateRegistration()
    {
        const uint philosopherId = 1;
        const string philosopherName = "Plato";

        var firstKey = await _dbManager.RegisterPhilosopherAsync(philosopherId, philosopherName);
        var secondKey = await _dbManager.RegisterPhilosopherAsync(philosopherId, "Different Name");

        Assert.Equal(firstKey, secondKey);

        var context = new SimulationDbContext(_options);
        var count = await context.Philosophers.CountAsync(p => p.Id == philosopherId);
        Assert.Equal(1, count);
    }

    [Fact]
    public async Task RegisterForkAsync_RegistersNewFork()
    {
        const uint forkId = 1;
        const string forkName = "Fork-1";

        var dbKey = await _dbManager.RegisterForkAsync(forkId, forkName);

        Assert.True(dbKey > 0);

        var context = new SimulationDbContext(_options);
        var fork = await context.Forks.FirstOrDefaultAsync(f => f.Key == dbKey);
        
        Assert.NotNull(fork);
        Assert.Equal(forkName, fork.Name);
        Assert.Equal(_dbManager.RunId, fork.RunId);
    }

    [Fact]
    public async Task RegisterForkAsync_ReturnsSameKeyForDuplicateRegistration()
    {
        const uint forkId = 1;
        const string forkName = "Fork-1";

        var firstKey = await _dbManager.RegisterForkAsync(forkId, forkName);
        var secondKey = await _dbManager.RegisterForkAsync(forkId, "Fork-Different");

        Assert.Equal(firstKey, secondKey);

        var context = new SimulationDbContext(_options);
        var count = await context.Forks.CountAsync();
        Assert.Equal(1, count);
    }

    [Fact]
    public async Task LogPhilosopherStateAsync_LogsState()
    {
        uint philosopherId = 1;
        await _dbManager.RegisterPhilosopherAsync(philosopherId, "Aristotle");
        var timestamp = DateTime.UtcNow;

        await _dbManager.LogPhilosopherStateAsync(philosopherId, "Thinking", 5, timestamp);

        var context = new SimulationDbContext(_options);
        var stateRecord = await context.PhilosopherStates
            .Include(ps => ps.Philosopher)
            .FirstOrDefaultAsync();

        Assert.NotNull(stateRecord);
        Assert.Equal("Thinking", stateRecord.State);
        Assert.Equal(5, stateRecord.Eaten);
        Assert.Equal(timestamp, stateRecord.Timestamp);
        Assert.Equal(philosopherId, stateRecord.Philosopher!.Id);
    }

    [Fact]
    public async Task LogPhilosopherStateAsync_ThrowsForUnregisteredPhilosopher()
    {
        const uint unregisteredId = 999;

        await Assert.ThrowsAsync<KeyNotFoundException>(
            () => _dbManager.LogPhilosopherStateAsync(unregisteredId, "Thinking", 0, DateTime.UtcNow)
        );
    }

    [Fact]
    public async Task LogForkStateAsync_LogsStateWithoutPhilosopher()
    {
        const uint forkId = 1;
        await _dbManager.RegisterForkAsync(forkId, "Fork-1");
        var timestamp = DateTime.UtcNow;

        await _dbManager.LogForkStateAsync(forkId, "Available", null, timestamp);

        var context = new SimulationDbContext(_options);
        var stateRecord = await context.ForkStates
            .Include(fs => fs.Fork)
            .FirstOrDefaultAsync();

        Assert.NotNull(stateRecord);
        Assert.Equal("Available", stateRecord.State);
        Assert.Null(stateRecord.TakenByPhilosopherId);
        Assert.Equal(timestamp, stateRecord.Timestamp);
    }

    [Fact]
    public async Task LogForkStateAsync_LogsStateWithPhilosopher()
    {
        const uint forkId = 1;
        const uint philosopherId = 1;
        await _dbManager.RegisterForkAsync(forkId, "Fork-1");
        await _dbManager.RegisterPhilosopherAsync(philosopherId, "Descartes");
        var timestamp = DateTime.UtcNow;

        await _dbManager.LogForkStateAsync(forkId, "Taken", philosopherId, timestamp);

        var context = new SimulationDbContext(_options);
        var stateRecord = await context.ForkStates
            .Include(fs => fs.Fork)
            .Include(fs => fs.TakenByPhilosopher)
            .FirstOrDefaultAsync();

        Assert.NotNull(stateRecord);
        Assert.Equal("Taken", stateRecord.State);
        Assert.NotNull(stateRecord.TakenByPhilosopherId);
        Assert.Equal(philosopherId, stateRecord.TakenByPhilosopher!.Id);
    }

    [Fact]
    public async Task LogForkStateAsync_ThrowsForUnregisteredFork()
    {
        const uint unregisteredForkId = 999;

        await Assert.ThrowsAsync<KeyNotFoundException>(
            () => _dbManager.LogForkStateAsync(unregisteredForkId, "Available", null, DateTime.UtcNow)
        );
    }

    [Fact]
    public async Task GetSnapshotAsync_ReturnsNullForNonexistentRun()
    {
        var nonexistentRunId = Guid.NewGuid();

        var snapshot = await _dbManager.GetSnapshotAsync(nonexistentRunId, 1.0);

        Assert.Null(snapshot);
    }

    [Fact]
    public async Task GetSnapshotAsync_ReturnsSnapshotWithLatestStates()
    {
        var philosopherId = 1u;
        var forkId = 1u;
        var startTime = _dbManager.StartTime;

        await _dbManager.RegisterPhilosopherAsync(philosopherId, "Kant");
        await _dbManager.RegisterForkAsync(forkId, "Fork-1");

        await _dbManager.LogPhilosopherStateAsync(philosopherId, "Thinking", 0, startTime.AddSeconds(1));
        await _dbManager.LogPhilosopherStateAsync(philosopherId, "Hungry", 0, startTime.AddSeconds(2));
        await _dbManager.LogPhilosopherStateAsync(philosopherId, "Eating", 1, startTime.AddSeconds(3));

        await _dbManager.LogForkStateAsync(forkId, "Available", null, startTime.AddSeconds(1));
        await _dbManager.LogForkStateAsync(forkId, "Taken", philosopherId, startTime.AddSeconds(2));

        var snapshot = await _dbManager.GetSnapshotAsync(_dbManager.RunId, 2.5);

        Assert.NotNull(snapshot);
        Assert.Equal(_dbManager.RunId, snapshot.RunId);
        Assert.Single(snapshot.PhilosopherStates);
        Assert.Single(snapshot.ForkStates);

        var philosopherState = snapshot.PhilosopherStates.First();
        Assert.Equal("Hungry", philosopherState.State);
        Assert.Equal(0, philosopherState.Eaten);

        var forkState = snapshot.ForkStates.First();
        Assert.Equal("Taken", forkState.State);
        Assert.NotNull(forkState.TakenByPhilosopherId);
    }

    [Fact]
    public async Task GetSnapshotAsync_HandlesMultiplePhilosophersAndForks()
    {
        var philosophers = new[] { (1u, "Hume"), (2u, "Locke"), (3u, "Berkeley") };
        var forks = new[] { 1u, 2u, 3u };

        foreach (var (id, name) in philosophers)
        {
            await _dbManager.RegisterPhilosopherAsync(id, name);
            await _dbManager.LogPhilosopherStateAsync(id, "Thinking", 0, _dbManager.StartTime.AddSeconds(1));
        }

        foreach (var forkId in forks)
        {
            await _dbManager.RegisterForkAsync(forkId, $"Fork-{forkId}");
            await _dbManager.LogForkStateAsync(forkId, "Available", null, _dbManager.StartTime.AddSeconds(1));
        }

        var snapshot = await _dbManager.GetSnapshotAsync(_dbManager.RunId, 1.5);

        Assert.NotNull(snapshot);
        Assert.Equal(3, snapshot.PhilosopherStates.Count);
        Assert.Equal(3, snapshot.ForkStates.Count);
    }
}