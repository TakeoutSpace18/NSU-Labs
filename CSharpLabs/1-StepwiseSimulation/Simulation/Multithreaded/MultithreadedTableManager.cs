using Contract;

namespace Simulation.Multithreaded;

public class MultithreadedTableManager : ITableManager
{
    private readonly List<MultithreadedFork> _forks = [];
    private readonly List<MultithreadedPhilosopher> _philosophers = [];
    public IEnumerable<IPhilosopher> Philosophers => _philosophers;
    public IEnumerable<IFork> Forks => _forks;

    public MultithreadedTableManager(string namesFilePath)
    {
        var names = new List<string>(File.ReadAllLines(namesFilePath));
        var count = names.Count;

        for (var i = 0; i < names.Count; i++)
        {
            _forks.Add(new MultithreadedFork($"Fork-{i + 1}"));
        }

        for (var i = 0; i < names.Count; i++)
        {
            _philosophers.Add(new MultithreadedPhilosopher(names[i], (uint)i, _forks[i % count], _forks[(i + 1) % count]));
        }
    }

    public void SetStrategy(IPhilosopherStrategy strategy)
    {
        _philosophers.ForEach(p => p.SetStrategy(strategy));
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