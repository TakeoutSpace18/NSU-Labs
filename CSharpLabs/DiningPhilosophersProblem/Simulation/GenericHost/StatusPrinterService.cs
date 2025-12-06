using System.Diagnostics;
using Contract;
using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Options;

namespace Simulation.GenericHost;

public class StatusPrinterService(ITableManager tableManager, IOptions<SimulationSettings> settings) : BackgroundService
{
    private readonly Stopwatch _stopwatch =  new();
    
    protected override async Task ExecuteAsync(CancellationToken stoppingToken)
    {
        _stopwatch.Start();
        while (!stoppingToken.IsCancellationRequested)
        {
            await Task.Delay(settings.Value.StatusUpdateIntervalMs, stoppingToken);
            PrintState(_stopwatch.ElapsedMilliseconds);
        }
    }
    
    private void PrintState(long timeElapsed)
    {
        Console.WriteLine($"===== ВРЕМЯ {timeElapsed} мс ===== ");

        Console.WriteLine("Философы:");
        foreach (var p in tableManager.Philosophers)
        {
            Console.WriteLine($"\t{p.StateString}");
        }

        Console.WriteLine("Вилки:");
        foreach (var f in tableManager.Forks)
        {
            Console.WriteLine($"\t{f.StateString}");
        }
    }
}