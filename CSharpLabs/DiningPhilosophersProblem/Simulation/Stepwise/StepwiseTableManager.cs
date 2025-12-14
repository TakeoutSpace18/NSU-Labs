using Contract;

namespace Simulation.Stepwise;

public class StepwiseTableManager : ITableManager
{
    private readonly List<StepwiseFork> _forks = [];
    private readonly List<StepwisePhilosopher> _philosophers = [];

    public IEnumerable<IPhilosopher> Philosophers => _philosophers;
    public IEnumerable<IFork> Forks => _forks;

    public StepwiseTableManager(string namesFilePath)
    {
        var names = new List<string>(File.ReadAllLines(namesFilePath));
        var count = names.Count;

        for (var i = 0; i < names.Count; i++)
        {
            _forks.Add(new StepwiseFork($"Fork-{i + 1}", (uint)i));
        }

        for (var i = 0; i < names.Count; i++)
        {
            _philosophers.Add(new StepwisePhilosopher(names[i], (uint)i, _forks[i % count], _forks[(i + 1) % count]));
        }
    }

    public void SetStrategy(IPhilosopherStrategy strategy)
    {
        _philosophers.ForEach(p => p.SetStrategy(strategy));
    }

    public void SetCoordinator(IPhilosopherCoordinator coordinator)
    {
        _philosophers.ForEach(p => SetCoordinator(coordinator));
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