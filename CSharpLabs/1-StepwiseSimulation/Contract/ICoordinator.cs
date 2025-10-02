namespace Contract;

public interface ICoordinator
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