using Contract;

namespace StrategyImpl;

public static class PhilosopherStrategyFactory
{
    public static IPhilosopherStrategy Create(string name)
    {
        return name switch
        {
            "naive" => new NaivePhilosopherStrategy(),
            "resource_hierarchy" => new ResourceHierarchyPhilosopherStrategy(),
            _ => throw new ArgumentOutOfRangeException(nameof(name))
        };
    }
}