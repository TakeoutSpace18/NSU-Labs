namespace Contract;

public interface ITableManager
{
    IPhilosopher GetPhilosopherAtIndex(int index);
    IFork GetForkAtIndex(int index);
    
    int PhilosopherCount { get; }
    
    IEnumerable<IPhilosopher> Philosophers { get; }
    IEnumerable<IFork> Forks { get; }
}