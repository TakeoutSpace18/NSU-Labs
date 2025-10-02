using System.Text;
using System.Text.Json;
using System.Text.Json.Serialization;
using Contract;
using StrategyImpl;

namespace Simulation;

public class Simulation
{
    public class NameList
    {
        public required string[] Names { get; init; }
    }


    private readonly List<Philosopher> _philosophers;
    private readonly List<Fork> _forks;
    private int _currentStep;

    private Simulation()
    {
        NameList namesList = JsonSerializer.Deserialize<NameList>(File.ReadAllText("names.json")) ??
                             throw new FormatException();

        _forks =
        [
            new Fork("Fork-1"),
            new Fork("Fork-2"),
            new Fork("Fork-3"),
            new Fork("Fork-4"),
            new Fork("Fork-5")
        ];

        _philosophers =
        [
            new Philosopher(namesList.Names[0], _forks[0], _forks[1]),
            new Philosopher(namesList.Names[1], _forks[1], _forks[2]),
            new Philosopher(namesList.Names[2], _forks[2], _forks[3]),
            new Philosopher(namesList.Names[3], _forks[3], _forks[4]),
            new Philosopher(namesList.Names[4], _forks[4], _forks[0])
        ];

        var strategy = new NaiveStrategy();
        var coordinator = new Coordinator(new List<IPhilosopher>(_philosophers));
        foreach (var philosopher in _philosophers)
        {
            //philosopher.SetCoordinator(coordinator);
            philosopher.SetStrategy(strategy);
        }

        _currentStep = 0;
    }

    private void Run(int steps)
    {
        var initialStep = _currentStep;
        while (_currentStep < initialStep + steps)
        {
            var stateChanged = false;

            foreach (var philosopher in _philosophers)
            {
                stateChanged |= philosopher.NextStep();
            }

            foreach (var fork in _forks)
            {
                fork.NextStep();
            }


            if (_philosophers.TrueForAll(p =>
                    p.CurrentState is Philosopher.State.WaitingLeftFork
                        or Philosopher.State.WaitingRightFork))
            {
                Console.WriteLine("Deadlock detected!");
                break;
            }

            _currentStep++;
            
            if (!stateChanged) continue;

            PrintState();
            Console.ReadLine();
        }

        var metrics = new Metrics(_philosophers, _forks, _currentStep);
        Console.Write(metrics.ToString());
    }


    private void PrintState()
    {
        Console.WriteLine($"===== ШАГ {_currentStep} ===== ");
        Console.WriteLine("Философы:");
        foreach (var philosopher in _philosophers)
        {
            Console.WriteLine($"\t{philosopher.StateString()}");
        }

        Console.WriteLine("Вилки:");
        foreach (var fork in _forks)
        {
            Console.WriteLine($"\t{fork.StateString()}");
        }
    }

    public bool NextStep()
    {
        var stateChanged = false;

        foreach (var philosopher in _philosophers)
        {
            stateChanged |= philosopher.NextStep();
        }

        _currentStep++;

        return stateChanged;
    }

    public static void Main()
    {
        var simulation = new Simulation();
        simulation.Run(1000000);
    }
}