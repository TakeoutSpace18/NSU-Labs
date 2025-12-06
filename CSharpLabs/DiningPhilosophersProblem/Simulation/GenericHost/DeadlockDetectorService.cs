using Contract;
using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Options;
using Simulation.DeadlockDetection;

namespace Simulation.GenericHost;

public class DeadlockDetectorService(
    ITableManager tableManager,
    IHostApplicationLifetime lifetime,
    IOptions<SimulationSettings> settings) : BackgroundService
{
    protected override async Task ExecuteAsync(CancellationToken stoppingToken)
    {
        while (!stoppingToken.IsCancellationRequested)
        {
            await Task.Delay(settings.Value.DeadlockCheckIntervalMs, stoppingToken);
            if (!DeadlockDetector.Check(tableManager)) continue;
            Console.BackgroundColor = ConsoleColor.Black;
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine("Deadlock detected!");
            Console.ResetColor();
            lifetime.StopApplication();
        }
    }
}