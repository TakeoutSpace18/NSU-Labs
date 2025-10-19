using Contract;

namespace StrategyImpl;

public class PhilosopherStrategyFactory
{
    public static IPhilosopherStrategy Create(string name)
    {
        return name switch
        {
            "naive" => new NaivePhilosopherStrategy(),
            _ => throw new ArgumentOutOfRangeException(nameof(name))
        };
    }
}