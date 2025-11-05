using System.Reflection;
using Contract;
using Simulation.Multithreaded;
using StrategyImpl;

namespace Simulation.Stepwise;

public class StepwiseSimulation : AbstractSimulation
{
    private int _currentStep = 0;

    private readonly StepwiseTableManager _tableManager
        = new("names.txt");

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

            foreach (var fork in _tableManager.Forks)
            {
                fork.NextStep();
            }

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
        foreach (var p in _tableManager.Philosophers)
        {
            Console.WriteLine($"\t{p.StateString}");
        }
        
        Console.WriteLine("Вилки:");
        foreach (var f in _tableManager.Forks)
        {
            Console.WriteLine($"\t{f.StateString}");
        }
    }
}