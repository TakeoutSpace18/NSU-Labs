using Contract;

namespace StrategyImpl;

public class SimplePhilosopherCoordinator(ITableManager tableManager) : IPhilosopherCoordinator
{
    public void DeclareHungry(IPhilosopher philosopher)
    {
        if (philosopher == tableManager.GetPhilosopherAtIndex(0))
        {
            if (!philosopher.HasLeftFork)
            {
                DoAction?.Invoke(philosopher, IPhilosopherCoordinator.Action.TakeLeftFork);
                return;
            }

            if (philosopher.HasRightFork) return;
            DoAction?.Invoke(philosopher, IPhilosopherCoordinator.Action.TakeRightFork);

            return;
        }

        if (!philosopher.HasRightFork)
        {
            DoAction?.Invoke(philosopher, IPhilosopherCoordinator.Action.TakeRightFork);
            return;
        }


        if (philosopher.HasLeftFork) return;
        DoAction?.Invoke(philosopher, IPhilosopherCoordinator.Action.TakeLeftFork);
        return;
    }

    public event Action<IPhilosopher, IPhilosopherCoordinator.Action>? DoAction;
}