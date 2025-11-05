namespace Contract;

public interface IPhilosopherStrategy
{
    void DoAction(IPhilosopher philosopher, CancellationToken? token = null);
}