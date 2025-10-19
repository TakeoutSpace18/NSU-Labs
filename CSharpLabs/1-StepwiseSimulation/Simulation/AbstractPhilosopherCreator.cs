using Contract;

namespace Simulation;

public abstract class AbstractPhilosopherCreator
{
    public abstract IPhilosopher Create(string name, IFork leftFork, IFork rightFork);
}