using Contract;

namespace Simulation.Stepwise;

public class StepwiseForkCreator : AbstractForkCreator
{
    public override IFork Create(string name)
    {
        return new StepwiseFork(name);
    }
}