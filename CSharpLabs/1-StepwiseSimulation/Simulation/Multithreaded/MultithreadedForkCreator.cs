using Contract;
using Simulation.Multithreaded;

namespace Simulation.Multithreaded;

public class MultithreadedForkCreator : AbstractForkCreator
{
    public override IFork Create(string name)
    {
        return new MultithreadedFork(name);
    }
}