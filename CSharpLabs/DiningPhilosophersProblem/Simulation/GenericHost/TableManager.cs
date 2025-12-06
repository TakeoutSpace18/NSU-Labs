using Contract;
using Simulation.Multithreaded;

namespace Simulation.GenericHost;

public class TableManager(List<MultithreadedPhilosopher> philosophers, List<MultithreadedFork> forks)
    : ITableManager
{
    public IEnumerable<IPhilosopher> Philosophers => philosophers;
    public IEnumerable<IFork> Forks => forks;

    public IPhilosopher GetPhilosopherAtIndex(int index)
    {
        return philosophers[index];
    }

    public IFork GetForkAtIndex(int index)
    {
        return forks[index];
    }

    public int PhilosopherCount => philosophers.Count;
}
