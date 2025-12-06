using Contract;
using Moq;
using StrategyImpl;
using Xunit;

namespace Test;

public class ResourceHierarchyPhilosopherStrategyTests
{
    private static Mock<IPhilosopher> MockPhilosopher(
        uint id, bool hasLeft, bool hasRight)
    {
        var mock = new Mock<IPhilosopher>();

        mock.Setup(p => p.Id).Returns(id);
        mock.Setup(p => p.HasLeftFork).Returns(hasLeft);
        mock.Setup(p => p.HasRightFork).Returns(hasRight);

        return mock;
    }

    // ID == 0: Left then Right

    [Fact]
    public void DoAction_IdZero_ShouldTakeLeftFirst_WhenLeftForkMissing()
    {
        var philosopher = MockPhilosopher(id: 0, hasLeft: false, hasRight: false);
        var strategy = new ResourceHierarchyPhilosopherStrategy();

        strategy.DoAction(philosopher.Object);

        philosopher.Verify(p => p.TakeLeftFork(It.IsAny<CancellationToken?>()), Times.Once);
        philosopher.Verify(p => p.TakeRightFork(It.IsAny<CancellationToken?>()), Times.Never);
        philosopher.Verify(p => p.PutLeftFork(), Times.Never);
        philosopher.Verify(p => p.PutRightFork(), Times.Never);
        philosopher.Verify(p => p.TryTakeLeftFork(), Times.Never);
        philosopher.Verify(p => p.TryTakeRightFork(), Times.Never);
    }

    [Fact]
    public void DoAction_IdZero_ShouldTakeRight_WhenLeftExistsButRightMissing()
    {
        var philosopher = MockPhilosopher(id: 0, hasLeft: true, hasRight: false);
        var strategy = new ResourceHierarchyPhilosopherStrategy();

        strategy.DoAction(philosopher.Object);

        philosopher.Verify(p => p.TakeLeftFork(It.IsAny<CancellationToken?>()), Times.Never);
        philosopher.Verify(p => p.TakeRightFork(It.IsAny<CancellationToken?>()), Times.Once);
        philosopher.Verify(p => p.PutLeftFork(), Times.Never);
        philosopher.Verify(p => p.PutRightFork(), Times.Never);
        philosopher.Verify(p => p.TryTakeLeftFork(), Times.Never);
        philosopher.Verify(p => p.TryTakeRightFork(), Times.Never);
    }

    [Fact]
    public void DoAction_IdZero_ShouldDoNothing_WhenBothForksExist()
    {
        var philosopher = MockPhilosopher(id: 0, hasLeft: true, hasRight: true);
        var strategy = new ResourceHierarchyPhilosopherStrategy();

        strategy.DoAction(philosopher.Object);

        philosopher.Verify(p => p.TakeLeftFork(It.IsAny<CancellationToken?>()), Times.Never);
        philosopher.Verify(p => p.TakeRightFork(It.IsAny<CancellationToken?>()), Times.Never);
        philosopher.Verify(p => p.PutLeftFork(), Times.Never);
        philosopher.Verify(p => p.PutRightFork(), Times.Never);
        philosopher.Verify(p => p.TryTakeLeftFork(), Times.Never);
        philosopher.Verify(p => p.TryTakeRightFork(), Times.Never);
    }

    // ID != 0 -> Right then Left

    [Fact]
    public void DoAction_IdNotZero_ShouldTakeRightFirst_WhenRightForkMissing()
    {
        var philosopher = MockPhilosopher(id: 1, hasLeft: false, hasRight: false);
        var strategy = new ResourceHierarchyPhilosopherStrategy();

        strategy.DoAction(philosopher.Object);

        philosopher.Verify(p => p.TakeRightFork(It.IsAny<CancellationToken?>()), Times.Once);
        philosopher.Verify(p => p.TakeLeftFork(It.IsAny<CancellationToken?>()), Times.Never);
        philosopher.Verify(p => p.PutLeftFork(), Times.Never);
        philosopher.Verify(p => p.PutRightFork(), Times.Never);
        philosopher.Verify(p => p.TryTakeLeftFork(), Times.Never);
        philosopher.Verify(p => p.TryTakeRightFork(), Times.Never);
    }

    [Fact]
    public void DoAction_IdNotZero_ShouldTakeLeft_WhenRightExistsButLeftMissing()
    {
        var philosopher = MockPhilosopher(id: 1, hasLeft: false, hasRight: true);
        var strategy = new ResourceHierarchyPhilosopherStrategy();

        strategy.DoAction(philosopher.Object);

        philosopher.Verify(p => p.TakeRightFork(It.IsAny<CancellationToken?>()), Times.Never);
        philosopher.Verify(p => p.TakeLeftFork(It.IsAny<CancellationToken?>()), Times.Once);
        philosopher.Verify(p => p.PutLeftFork(), Times.Never);
        philosopher.Verify(p => p.PutRightFork(), Times.Never);
        philosopher.Verify(p => p.TryTakeLeftFork(), Times.Never);
        philosopher.Verify(p => p.TryTakeRightFork(), Times.Never);
    }

    [Fact]
    public void DoAction_IdNotZero_ShouldDoNothing_WhenBothForksExist()
    {
        var philosopher = MockPhilosopher(id: 1, hasLeft: true, hasRight: true);
        var strategy = new ResourceHierarchyPhilosopherStrategy();

        strategy.DoAction(philosopher.Object);

        philosopher.Verify(p => p.TakeLeftFork(It.IsAny<CancellationToken?>()), Times.Never);
        philosopher.Verify(p => p.TakeRightFork(It.IsAny<CancellationToken?>()), Times.Never);
        philosopher.Verify(p => p.PutLeftFork(), Times.Never);
        philosopher.Verify(p => p.PutRightFork(), Times.Never);
        philosopher.Verify(p => p.TryTakeLeftFork(), Times.Never);
        philosopher.Verify(p => p.TryTakeRightFork(), Times.Never);
    }
}