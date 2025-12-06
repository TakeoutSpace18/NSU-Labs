namespace Contract;

public interface IPhilosopherCoordinator
{
    enum Action
    {
        TakeLeftFork,
        TakeRightFork,
        ReleaseLeftFork,
        ReleaseRightFork,
    }
    void DeclareHungry(IPhilosopher philosopher);

    event Action<IPhilosopher, Action> DoAction;
}