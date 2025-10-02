using Contract;

namespace StrategyImpl;

public class NaiveStrategy : IStrategy
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