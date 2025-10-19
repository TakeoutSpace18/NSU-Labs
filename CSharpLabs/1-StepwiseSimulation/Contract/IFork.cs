namespace Contract;

public interface IFork
{
    bool NextStep();
    bool IsAvailable { get; }
    
    int AvailableTime { get; }
    int TakenTime { get; }
    int InUseTime { get; }
    
    string Name { get; }

    bool TryTake(IPhilosopher philosopher);
    void Take(IPhilosopher philosopher);
    void Use(IPhilosopher philosopher);
    void Put();

    string StateString { get; }
}