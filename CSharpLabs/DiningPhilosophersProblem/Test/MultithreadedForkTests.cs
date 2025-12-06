using Contract;
using Simulation.Multithreaded;

namespace Test;

using Xunit;
using Moq;

public class MultithreadedForkTests
{
    [Fact]
    public void TryTake_WhenAvailable_ShouldReturnTrue_AndChangeStateToTaken()
    {
        var fork = new MultithreadedFork("F1");
        var p = Mock.Of<IPhilosopher>();

        var result = fork.TryTake(p);

        Assert.True(result);
        Assert.Equal(MultithreadedFork.State.Taken, fork.CurrentState);
    }

    [Fact]
    public void TryTake_WhenUnavailable_ShouldReturnFalse()
    {
        var fork = new MultithreadedFork("F1");
        var p = Mock.Of<IPhilosopher>();

        // first take
        Assert.True(fork.TryTake(p));

        // second should fail
        var result = fork.TryTake(p);

        Assert.False(result);
    }

    [Fact]
    public void Use_AfterTake_ShouldChangeStateToInUse()
    {
        var fork = new MultithreadedFork("F1");
        var p = Mock.Of<IPhilosopher>();

        var result = fork.TryTake(p);
        fork.Use(p);

        Assert.True(result);
        Assert.Equal(MultithreadedFork.State.InUse, fork.CurrentState);
    }

    [Fact]
    public void Put_AfterUse_ShouldResetStateToAvailable_AndReleaseSemaphore()
    {
        var fork = new MultithreadedFork("F1");
        var p = Mock.Of<IPhilosopher>();

        fork.TryTake(p);
        fork.Use(p);
        fork.Put();

        Assert.Equal(MultithreadedFork.State.Available, fork.CurrentState);
        Assert.True(fork.InUseTime >= 0);
        Assert.True(fork.TakenTime >= 0);
    }

    [Fact]
    public void Take_ShouldAcquireForkAndSetStateToTaken()
    {
        var fork = new MultithreadedFork("F1");
        var philosopher = new Mock<IPhilosopher>().Object;

        fork.Take(philosopher);

        Assert.Equal(MultithreadedFork.State.Taken, fork.CurrentState);
    }

    [Fact]
    public void Take_WithCancellation_ShouldThrowIfTokenCancelled()
    {
        var fork = new MultithreadedFork("F1");
        var philosopher = new Mock<IPhilosopher>().Object;

        fork.Take(philosopher); // first take

        using var cts = new CancellationTokenSource();
        cts.Cancel();

        Assert.Throws<OperationCanceledException>(() => fork.Take(philosopher, cts.Token));
    }
}