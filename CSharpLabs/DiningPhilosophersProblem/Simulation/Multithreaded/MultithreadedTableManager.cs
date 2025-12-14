using Contract;
using Simulation.Database;
using StrategyImpl;

namespace Simulation.Multithreaded;

public class MultithreadedTableManager : ITableManager
{
    private readonly List<MultithreadedFork> _forks = [];
    private readonly List<MultithreadedPhilosopher> _philosophers = [];
    public IEnumerable<IPhilosopher> Philosophers => _philosophers;
    public IEnumerable<IFork> Forks => _forks;

    public MultithreadedTableManager(SimulationSettings simulationSettings)
    {
        var names = simulationSettings.PhilosopherNames;
        var count = names.Count;

        for (var i = 0; i < count; i++)
        {
            _forks.Add(new MultithreadedFork($"Fork-{i + 1}", (uint)i, null));
        }

        for (var i = 0; i < count; i++)
        {
            var strategy = PhilosopherStrategyFactory.Create(simulationSettings.PhilosopherStrategy);
            _philosophers.Add(new MultithreadedPhilosopher(names[i], (uint)i, _forks[i % count],
                _forks[(i + 1) % count], strategy, simulationSettings.ActionTimes, null));
        }
    }

    public IPhilosopher GetPhilosopherAtIndex(int index)
    {
        return _philosophers[index];
    }

    public IFork GetForkAtIndex(int index)
    {
        return _forks[index];
    }

    public int PhilosopherCount => _philosophers.Count;
}