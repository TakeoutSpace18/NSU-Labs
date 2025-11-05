using Contract;

namespace StrategyImpl;

public class SimplePhilosopherCoordinator(ITableManager tableManager) : IPhilosopherCoordinator
{
    public event Action<IPhilosopher, IPhilosopherCoordinator.Action>? DoAction;
    
    public void DeclareHungry(IPhilosopher philosopher)
    {
        if (philosopher.Id == 0)
        {
            TakeLeftRight(philosopher);
        }
        else
        {
            TakeRightLeft(philosopher);
        }
    }
    
    private void TakeLeftRight(IPhilosopher p)
    {
        
        if (!p.HasLeftFork)
        {
            DoAction?.Invoke(p, IPhilosopherCoordinator.Action.TakeLeftFork);
            return;
        }

        if (p.HasRightFork) return;
        DoAction?.Invoke(p, IPhilosopherCoordinator.Action.TakeRightFork);
    }

    private void TakeRightLeft(IPhilosopher p)
    {
        
        if (!p.HasRightFork)
        {
            DoAction?.Invoke(p, IPhilosopherCoordinator.Action.TakeRightFork);
            return;
        }

        if (p.HasLeftFork) return;
        DoAction?.Invoke(p, IPhilosopherCoordinator.Action.TakeLeftFork);
    }
}