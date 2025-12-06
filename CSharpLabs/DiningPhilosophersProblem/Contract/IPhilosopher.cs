namespace Contract;

public interface IPhilosopher
{
    bool NextStep(CancellationToken? token = null);
    
    bool HasLeftFork { get; }
    bool HasRightFork { get; }
    bool WaitsLeftFork { get;  }
    bool WaitsRightFork { get;  }
   
    void TakeLeftFork(CancellationToken? token = null);
    void TakeRightFork(CancellationToken? token = null);
    
    bool TryTakeLeftFork();
    bool TryTakeRightFork();
    
    void PutLeftFork();
    void PutRightFork();
    
    long Eaten { get; }
    long WaitingTime { get; }
    string Name { get; }
    uint Id { get; }
    string StateString { get; }
}