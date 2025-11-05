using Contract;

namespace StrategyImpl;

public class ResourceHierarchyPhilosopherStrategy : IPhilosopherStrategy
{
    public void DoAction(IPhilosopher philosopher, CancellationToken? token = null)
    {
        if (philosopher.Id == 0)
        {
            TakeLeftRight(philosopher, token);
        }
        else
        {
            TakeRightLeft(philosopher, token);
        }
    }

    private static void TakeLeftRight(IPhilosopher p, CancellationToken? token)
    {
        if (!p.HasLeftFork)
        {
            p.TakeLeftFork(token);
            return;
        }

        if (p.HasRightFork) return;
        p.TakeRightFork(token);
    }

    private static void TakeRightLeft(IPhilosopher p, CancellationToken? token)
    {
        if (!p.HasRightFork)
        {
            p.TakeRightFork(token);
            return;
        }

        if (p.HasLeftFork) return;
        p.TakeLeftFork(token);
        
    }
}