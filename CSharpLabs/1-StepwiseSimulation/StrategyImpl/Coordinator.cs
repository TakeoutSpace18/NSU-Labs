using Contract;

namespace StrategyImpl;

public class Coordinator(List<IPhilosopher> philosophers) : ICoordinator
{
    public void DeclareHungry(IPhilosopher philosopher)
    {
        if (philosopher == philosophers[0])
        {
            if (!philosopher.HasLeftFork)
            {
                DoAction?.Invoke(philosopher, ICoordinator.Action.TakeLeftFork);
                return;
            }

            if (!philosopher.HasRightFork)
            {
                DoAction?.Invoke(philosopher, ICoordinator.Action.TakeRightFork);
                return;
            }

            return;
        }

        if (!philosopher.HasRightFork)
        {
            DoAction?.Invoke(philosopher, ICoordinator.Action.TakeRightFork);
            return;
        }


        if (!philosopher.HasLeftFork)
        {
            DoAction?.Invoke(philosopher, ICoordinator.Action.TakeLeftFork);
            return;
        }
    }

    public event Action<IPhilosopher, ICoordinator.Action>? DoAction;
}