using System.Diagnostics;
using Contract;
using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Options;

namespace Simulation.GenericHost;

public class TimedShutdownService(
    ITableManager tableManager,
    IHostApplicationLifetime lifetime,
    IOptions<SimulationSettings> settings) : BackgroundService
{
    private readonly Stopwatch _stopwatch = new();

    protected override async Task ExecuteAsync(CancellationToken stoppingToken)
    {
        _stopwatch.Start();

        lifetime.ApplicationStopped.Register(() =>
        {
            var collector = new MetricsCollector(tableManager, _stopwatch.ElapsedMilliseconds);
            Console.WriteLine(collector.ToString());
        });
        
        try
        {
            await Task.Delay(TimeSpan.FromSeconds(settings.Value.DurationSeconds), stoppingToken);
        }
        catch (TaskCanceledException)
        {
            return;
        }

        lifetime.StopApplication();
    }
}