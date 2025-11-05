namespace Contract;

public interface IFork
{
    bool NextStep();
    
    long AvailableTime { get; }
    long TakenTime { get; }
    long InUseTime { get; }
    
    string Name { get; }

    bool TryTake(IPhilosopher philosopher);
    void Take(IPhilosopher philosopher, CancellationToken? token = null);
    void Use(IPhilosopher philosopher);
    void Put();

    string StateString { get; }
}