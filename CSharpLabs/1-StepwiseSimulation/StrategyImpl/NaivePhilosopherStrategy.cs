using Contract;

namespace StrategyImpl;

public class NaivePhilosopherStrategy : IPhilosopherStrategy
{
    public void DoAction(IPhilosopher philosopher)
    {
        if (!philosopher.HasLeftFork)
        {
            philosopher.TakeLeftFork();
            return;
        }

        if (!philosopher.HasRightFork)
        {
            philosopher.TakeRightFork();
            return;
        }
    }
}