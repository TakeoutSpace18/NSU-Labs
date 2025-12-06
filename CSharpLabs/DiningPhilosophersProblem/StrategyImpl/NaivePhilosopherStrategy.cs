using Contract;

namespace StrategyImpl;

public class NaivePhilosopherStrategy : IPhilosopherStrategy
{
    public void DoAction(IPhilosopher philosopher, CancellationToken? token = null)
    {
        if (!philosopher.HasLeftFork)
        {
            philosopher.TakeLeftFork(token);
            return;
        }

        if (!philosopher.HasRightFork)
        {
            philosopher.TakeRightFork(token);
            return;
        }
    }
}