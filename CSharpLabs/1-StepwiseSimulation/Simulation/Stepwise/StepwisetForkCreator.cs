using Contract;

namespace Simulation.Stepwise;

public class StepwisetForkCreator : AbstractForkCreator
{
    public override IFork Create(string name)
    {
        return new StepwiseFork(name);
    }
}