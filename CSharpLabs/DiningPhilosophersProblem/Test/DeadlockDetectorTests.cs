using Contract;
using Simulation.DeadlockDetection;

namespace Test;

using System.Collections.Generic;
using System.Linq;
using Moq;
using Xunit;

public class DeadlockDetectorTests
{
    private static Mock<IPhilosopher> MockPhilosopher(
        bool waitsLeft, bool waitsRight, bool hasLeft, bool hasRight)
    {
        var mock = new Mock<IPhilosopher>();
        mock.Setup(p => p.WaitsLeftFork).Returns(waitsLeft);
        mock.Setup(p => p.WaitsRightFork).Returns(waitsRight);
        mock.Setup(p => p.HasLeftFork).Returns(hasLeft);
        mock.Setup(p => p.HasRightFork).Returns(hasRight);
        return mock;
    }

    private static Mock<ITableManager> MockTable(IEnumerable<Mock<IPhilosopher>> philosophers)
    {
        var table = new Mock<ITableManager>();
        table.Setup(t => t.Philosophers).Returns(philosophers.Select(m => m.Object));
        return table;
    }

    // CASE 1: True deadlock 
    [Fact]
    public void Check_ShouldReturnTrue_WhenAllPhilosophersWaitingAndNoneEating()
    {
        // all philosophers waiting + none has both forks
        var p1 = MockPhilosopher(waitsLeft: true, waitsRight: false, hasLeft: false, hasRight: false);
        var p2 = MockPhilosopher(waitsLeft: false, waitsRight: true, hasLeft: false, hasRight: false);
        var table = MockTable([p1, p2]);

        var result = DeadlockDetector.Check(table.Object);

        Assert.True(result);
    }

    // CASE 2: Not deadlock (someone is eating)
    [Fact]
    public void Check_ShouldReturnFalse_WhenAnyPhilosopherIsEating()
    {
        // one philosopher has both forks -> eating
        var p1 = MockPhilosopher(waitsLeft: true, waitsRight: true, hasLeft: true, hasRight: true);
        var p2 = MockPhilosopher(waitsLeft: true, waitsRight: true, hasLeft: false, hasRight: true);
        var table = MockTable([p1, p2]);

        var result = DeadlockDetector.Check(table.Object);

        Assert.False(result);
    }

    // CASE 3: Not deadlock (a philosopher is NOT waiting)
    [Fact]
    public void Check_ShouldReturnFalse_WhenAnyPhilosopherIsNotWaiting()
    {
        // one philosopher is not waiting
        var p1 = MockPhilosopher(waitsLeft: false, waitsRight: false, hasLeft: false, hasRight: false);
        var p2 = MockPhilosopher(waitsLeft: true, waitsRight: true, hasLeft: false, hasRight: false);
        var table = MockTable([p1, p2]);

        var result = DeadlockDetector.Check(table.Object);

        Assert.False(result);
    }

    // CASE 4: Edge Case – Empty Table
    [Fact]
    public void Check_ShouldReturnTrue_WhenThereAreNoPhilosophers()
    {
        var table = MockTable([]);

        var result = DeadlockDetector.Check(table.Object);

        Assert.True(result);
    }
}