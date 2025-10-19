namespace Contract;

public interface IPhilosopher
{
    bool NextStep();
    
    IFork LeftFork { get; }
    IFork RightFork { get; }
    
    bool HasLeftFork { get; }
    bool HasRightFork { get; }
   
    void TakeLeftFork();
    void TakeRightFork();
    
    bool TryTakeLeftFork();
    bool TryTakeRightFork();
    
    void PutLeftFork();
    void PutRightFork();
    
    void SetStrategy(IPhilosopherStrategy strategy);
    void SetCoordinator(IPhilosopherCoordinator philosopherCoordinator);
    
    int Eaten { get; }
    int WaitingTime { get; }
    string Name { get; }
    string StateString { get; }
}