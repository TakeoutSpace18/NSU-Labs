using Contract;
using Simulation.Stepwise;

namespace Simulation;

public class StepwisePhilosopherCreator :  AbstractPhilosopherCreator
{
    public override IPhilosopher Create(string name, IFork leftFork, IFork rightFork)
    {
        return new StepwisePhilosopher(name, leftFork, rightFork);
    }
}