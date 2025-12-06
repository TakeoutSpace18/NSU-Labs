using System.Diagnostics;
using Simulation.DeadlockDetection;
using StrategyImpl;

namespace Simulation.Multithreaded;

public class MultithreadedSimulation : AbstractSimulation
{
    private readonly MultithreadedTableManager _tableManager;

    public MultithreadedSimulation()
    {
        if (SimulationSettings.PreferCoordinator)
            throw new NotImplementedException("Coordinator is not supported in multithreaded simulation");
        
        _tableManager = new MultithreadedTableManager(SimulationSettings);
    }

    public override void Run(int time)
    {
        var cts = new CancellationTokenSource(time);
        var token = cts.Token;

        List<Task> tasks = [];
        foreach (var p in _tableManager.Philosophers)
        {
            tasks.Add(Task.Run(() =>
            {
                while (!token.IsCancellationRequested)
                {
                    try
                    {
                        p.NextStep(token);
                    }
                    catch (OperationCanceledException)
                    {
                    }
                }

                Console.WriteLine("task finished");
            }, token));
        }

        Task.Run(() =>
        {
            while (!token.IsCancellationRequested)
            {
                Task.Delay(100).Wait();
                if (!DeadlockDetector.Check(_tableManager)) continue;
                
                Console.WriteLine("DEADLOCK DETECTED!");
                cts.Cancel();
            }
        }, token);

        if (SimulationSettings.DisplayStatus)
        {
            var stopwatch = new Stopwatch();
            stopwatch.Start();
            while (stopwatch.ElapsedMilliseconds < time)
            {
                PrintState(stopwatch.ElapsedMilliseconds);
                Task.Delay(300).Wait();
            }
        }

        foreach (var task in tasks)
        {
            task.Wait();
        }

        var metrics = new MetricsCollector(_tableManager, time);
        Console.Write(metrics.ToString());
    }

    private void PrintState(long timeElapsed)
    {
        Console.WriteLine($"===== ВРЕМЯ {timeElapsed} мс ===== ");

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