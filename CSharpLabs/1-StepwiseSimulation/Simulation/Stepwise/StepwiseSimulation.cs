using System.Reflection;
using Contract;
using StrategyImpl;

namespace Simulation.Stepwise;

public class StepwiseSimulation : AbstractSimulation
{
    private int _currentStep = 0;

    private readonly TableManager _tableManager
        = new("names.txt", new StepwisePhilosopherCreator(), new StepwisetForkCreator());

    public StepwiseSimulation()
    {
        if (SimulationSettings.PreferCoordinator)
        {
            var name = SimulationSettings.PhilosopherCoordinator;
            var coordinator = PhilosopherCoordinatorFactory.Create(name, _tableManager);

            _tableManager.SetCoordinator(coordinator);
        }
        else
        {
            var name = SimulationSettings.PhilosopherStrategy;
            var strategy = PhilosopherStrategyFactory.Create(name);

            _tableManager.SetStrategy(strategy);
        }
    }

    public override void Run(int time)
    {
        var initialStep = _currentStep;
        while (_currentStep < initialStep + time)
        {
            var stateChanged =
                _tableManager.Philosophers.Aggregate(false, (changed, philosopher) => changed | philosopher.NextStep());

            _tableManager.Forks.ForEach(f => f.NextStep());

            _currentStep++;

            if (!SimulationSettings.PrintStateChanges) continue;

            if (!stateChanged) continue;

            PrintState();
            Console.ReadLine();
        }

        var metrics = new MetricsCollector(_tableManager, _currentStep);
        Console.Write(metrics.ToString());
    }


    private void PrintState()
    {
        Console.WriteLine($"===== ШАГ {_currentStep} ===== ");
        Console.WriteLine("Философы:");
        _tableManager.Philosophers.ForEach(p => Console.WriteLine($"\t{p.StateString}"));

        Console.WriteLine("Вилки:");
        _tableManager.Forks.ForEach(f => Console.WriteLine($"\t{f.StateString}"));
    }
}