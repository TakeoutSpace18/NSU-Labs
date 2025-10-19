using Contract;

namespace StrategyImpl;

public class PhilosopherCoordinatorFactory
{
    public static IPhilosopherCoordinator Create(string name, ITableManager tableManager)
    {
        return name switch
        {
            "simple" => new SimplePhilosopherCoordinator(tableManager),
            _ => throw new ArgumentOutOfRangeException(nameof(name))
        };
    }
    
}