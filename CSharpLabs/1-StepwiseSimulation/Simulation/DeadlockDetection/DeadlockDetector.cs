using Contract;

namespace Simulation.DeadlockDetection;

public static class DeadlockDetector
{
    public static bool Check(ITableManager tableManager)
    {
        var allWaiting = tableManager.Philosophers.All(p => p.WaitsLeftFork || p.WaitsRightFork);
        var noneEating = tableManager.Philosophers.All(p => !p.HasLeftFork || !p.HasRightFork);

        return allWaiting && noneEating;
    }
}