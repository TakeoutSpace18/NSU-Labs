using Contract;

namespace Simulation;

public class TableManager : ITableManager
{
    public TableManager(string namesFilePath, AbstractPhilosopherCreator abstractPhilosopherCreator, AbstractForkCreator abstractForkCreator)
    {
        var names = new List<string>(File.ReadAllLines(namesFilePath));
        var count = names.Count;

        for (var i = 0; i < names.Count; i++)
        {
            Forks.Add(abstractForkCreator.Create($"Fork-{i + 1}"));
        }

        for (var i = 0; i < names.Count; i++)
        {
            Philosophers.Add(abstractPhilosopherCreator.Create(names[i], Forks[i % count], Forks[(i + 1) % count]));
        }
    }

    public void SetStrategy(IPhilosopherStrategy strategy)
    {
        Philosophers.ForEach(p => p.SetStrategy(strategy));
    }

    public void SetCoordinator(IPhilosopherCoordinator coordinator)
    {
        Philosophers.ForEach(p => SetCoordinator(coordinator));
    }

    public IPhilosopher GetPhilosopherAtIndex(int index)
    {
        return Philosophers[index];
    }

    public IFork GetForkAtIndex(int index)
    {
        return Forks[index];
    }

    public int PhilosopherCount => Philosophers.Count;
    public List<IPhilosopher> Philosophers { get; } = [];

    public List<IFork> Forks { get; } = [];
}