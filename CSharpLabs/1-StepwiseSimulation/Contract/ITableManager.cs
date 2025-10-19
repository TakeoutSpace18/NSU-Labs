namespace Contract;

public interface ITableManager
{
    IPhilosopher GetPhilosopherAtIndex(int index);
    IFork GetForkAtIndex(int index);
    
    int PhilosopherCount { get; }
    
    List<IPhilosopher> Philosophers { get; }
    List<IFork> Forks { get; }
    
    void SetStrategy(IPhilosopherStrategy strategy);
    void SetCoordinator(IPhilosopherCoordinator coordinator);
}