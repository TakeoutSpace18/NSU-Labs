using Contract;
using Moq;
using Simulation.Multithreaded;
using Xunit;

namespace Test;

public class MultithreadedPhilosopherTests
{
    [Fact]
    public void NextStep_FromThinking_ShouldTransitionToHungry()
    {
        var left = new MultithreadedFork("L", 0, null);
        var right = new MultithreadedFork("R", 0, null);
        var strategy = new Mock<IPhilosopherStrategy>();
        var p = new MultithreadedPhilosopher(
            "P1", 1, left, right, strategy.Object, TestActionTimes.Fast, null);

        Assert.True(p.NextStep()); // Thinking -> Hungry
        Assert.Equal(MultithreadedPhilosopher.State.Hungry, p.CurrentState);
    }

    [Fact]
    public void NextStep_Hungry_ShouldCallStrategy_IfNoForks()
    {
        var left = new MultithreadedFork("L", 0, null);
        var right = new MultithreadedFork("R", 0, null);
        var strategy = new Mock<IPhilosopherStrategy>();
        var p = new MultithreadedPhilosopher(
            "P1", 1, left, right, strategy.Object, TestActionTimes.Fast, null);

        p.NextStep(); // Thinking -> Hungry
        p.NextStep(); // Hungry -> strategy.DoAction()

        strategy.Verify(s => s.DoAction(p, It.IsAny<CancellationToken?>()), Times.Once);
    }

    [Fact]
    public void TakeLeftFork_ShouldTransitionToTakingLeftFork_ThenHungry()
    {
        var left = new MultithreadedFork("L", 0, null);
        var right = new MultithreadedFork("R", 0, null);
        var strategy = new Mock<IPhilosopherStrategy>();
        var p = new MultithreadedPhilosopher(
            "P1", 1, left, right, strategy.Object, TestActionTimes.Fast, null);

        p.NextStep(); // Thinking -> Hungry
        p.TakeLeftFork();

        Assert.Equal(MultithreadedPhilosopher.State.TakingLeftFork, p.CurrentState);

        p.NextStep(); // complete the fork acquisition

        Assert.Equal(MultithreadedPhilosopher.State.Hungry, p.CurrentState);
        Assert.True(p.HasLeftFork);
    }

    [Fact]
    public void TakeRightFork_ShouldTransitionToTakingRightFork_ThenHungry()
    {
        var left = new MultithreadedFork("L", 0, null);
        var right = new MultithreadedFork("R", 0, null);
        var strategy = new Mock<IPhilosopherStrategy>();
        var p = new MultithreadedPhilosopher(
            "P1", 1, left, right, strategy.Object, TestActionTimes.Fast, null);

        p.NextStep(); // Thinking -> Hungry
        p.TakeRightFork();

        Assert.Equal(MultithreadedPhilosopher.State.TakingRightFork, p.CurrentState);

        p.NextStep();

        Assert.Equal(MultithreadedPhilosopher.State.Hungry, p.CurrentState);
        Assert.True(p.HasRightFork);
    }
    
    [Fact]
    public void NextStep_Hungry_AndHasBothForks_ShouldTransitionToEating()
    {
        var left = new MultithreadedFork("L", 0, null);
        var right = new MultithreadedFork("R", 0, null);
        var strategy = new Mock<IPhilosopherStrategy>();
        var p = new MultithreadedPhilosopher(
            "P1", 1, left, right, strategy.Object, TestActionTimes.Fast, null);

        p.NextStep(); // Thinking -> Hungry

        p.TakeLeftFork();
        p.NextStep();

        p.TakeRightFork();
        p.NextStep();

        p.NextStep(); // Hungry -> Eating

        Assert.Equal(MultithreadedPhilosopher.State.Eating, p.CurrentState);
        Assert.Equal(1, p.Eaten);
    }
    
    [Fact]
    public void NextStep_Eating_ShouldReturnToThinking_AndReleaseForks()
    {
        var left = new MultithreadedFork("L", 0, null);
        var right = new MultithreadedFork("R", 0, null);
        var strategy = new Mock<IPhilosopherStrategy>();
        var p = new MultithreadedPhilosopher(
            "P1", 1, left, right, strategy.Object, TestActionTimes.Fast, null);

        p.NextStep(); // Thinking -> Hungry

        p.TakeLeftFork(); p.NextStep(); // acquire left
        p.TakeRightFork(); p.NextStep(); // acquire right

        p.NextStep(); // Hungry -> Eating

        Assert.Equal(MultithreadedPhilosopher.State.Eating, p.CurrentState);

        p.NextStep(); // Eating -> Thinking

        Assert.Equal(MultithreadedPhilosopher.State.Thinking, p.CurrentState);
        Assert.False(p.HasLeftFork);
        Assert.False(p.HasRightFork);
    }
}