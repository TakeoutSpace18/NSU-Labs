using Contract;
using Simulation.Stepwise;

namespace Simulation;

public abstract class AbstractForkCreator
{
    public abstract IFork Create(string name);
}