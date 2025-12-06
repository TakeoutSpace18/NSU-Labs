using Contract;
using Moq;
using StrategyImpl;
using Xunit;

namespace Test;

public class NaivePhilosopherStrategyTests
{
    private static Mock<IPhilosopher> MockPhilosopher(
        bool hasLeft, bool hasRight)
    {
        var mock = new Mock<IPhilosopher>();

        mock.Setup(p => p.HasLeftFork).Returns(hasLeft);
        mock.Setup(p => p.HasRightFork).Returns(hasRight);

        return mock;
    }

    [Fact]
    public void DoAction_ShouldTakeLeftFork_WhenLeftForkIsMissing()
    {
        var philosopher = MockPhilosopher(hasLeft: false, hasRight: false);
        var strategy = new NaivePhilosopherStrategy();

        strategy.DoAction(philosopher.Object);

        philosopher.Verify(p => p.TakeLeftFork(It.IsAny<CancellationToken?>()), Times.Once);
        philosopher.Verify(p => p.TakeRightFork(It.IsAny<CancellationToken?>()), Times.Never);
        philosopher.Verify(p => p.PutLeftFork(), Times.Never);
        philosopher.Verify(p => p.PutRightFork(), Times.Never);
        philosopher.Verify(p => p.TryTakeLeftFork(), Times.Never);
        philosopher.Verify(p => p.TryTakeRightFork(), Times.Never);
    }

    [Fact]
    public void DoAction_ShouldTakeRightFork_WhenLeftExistsButRightIsMissing()
    {
        var philosopher = MockPhilosopher(hasLeft: true, hasRight: false);
        var strategy = new NaivePhilosopherStrategy();

        strategy.DoAction(philosopher.Object);

        philosopher.Verify(p => p.TakeLeftFork(It.IsAny<CancellationToken?>()), Times.Never);
        philosopher.Verify(p => p.TakeRightFork(It.IsAny<CancellationToken?>()), Times.Once);
        philosopher.Verify(p => p.PutLeftFork(), Times.Never);
        philosopher.Verify(p => p.PutRightFork(), Times.Never);
        philosopher.Verify(p => p.TryTakeLeftFork(), Times.Never);
        philosopher.Verify(p => p.TryTakeRightFork(), Times.Never);
    }

    [Fact]
    public void DoAction_ShouldDoNothing_WhenPhilosopherHasBothForks()
    {
        var philosopher = MockPhilosopher(hasLeft: true, hasRight: true);
        var strategy = new NaivePhilosopherStrategy();

        strategy.DoAction(philosopher.Object);

        philosopher.Verify(p => p.TakeLeftFork(It.IsAny<CancellationToken?>()), Times.Never);
        philosopher.Verify(p => p.TakeRightFork(It.IsAny<CancellationToken?>()), Times.Never);
        philosopher.Verify(p => p.PutLeftFork(), Times.Never);
        philosopher.Verify(p => p.PutRightFork(), Times.Never);
        philosopher.Verify(p => p.TryTakeLeftFork(), Times.Never);
        philosopher.Verify(p => p.TryTakeRightFork(), Times.Never);
    }
}