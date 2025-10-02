namespace Contract;

public interface IPhilosopher
{
    IFork LeftFork { get; }
    IFork RightFork { get; }
    
    bool HasLeftFork { get; }
    bool HasRightFork { get; }
   
    void TakeLeftFork();
    void TakeRightFork();
    
    void ReleaseLeftFork();
    void ReleaseRightFork();
}