using Contract;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Simulation.Multithreaded;
using StrategyImpl;

namespace Simulation.GenericHost;

public static class HostSetup
{
    public static IHost Build(string[]? args)
    {
        var builder = Host.CreateDefaultBuilder(args)
            .ConfigureAppConfiguration((ctx, conf) =>
            {
                conf.AddJsonFile("appsettings.json", optional: true, reloadOnChange: true);
            })
            .ConfigureServices((ctx, services) =>
            {
                var settings = ctx.Configuration.GetSection("SimulationSettings").Get<SimulationSettings>() ??
                               throw new ArgumentNullException();

                services.Configure<SimulationSettings>(ctx.Configuration.GetSection("SimulationSettings"));

                services.AddSingleton<IPhilosopherStrategy>(sp =>
                    PhilosopherStrategyFactory.Create(settings.PhilosopherStrategy));
                
                var philosopherNames = settings.PhilosopherNames;

                services.AddSingleton(sp =>
                {
                    var forks = new List<MultithreadedFork>();
                    for (var i = 0; i < philosopherNames.Count; i++)
                        forks.Add(new MultithreadedFork($"Fork-{i}"));
                    return forks;
                });

                services.AddSingleton<List<MultithreadedPhilosopher>>();
                for (var i = 0; i < philosopherNames.Count; i++)
                {
                    var index = i;
                    services.AddSingleton<IHostedService>(sp =>
                    {
                        var forks = sp.GetRequiredService<List<MultithreadedFork>>();
                        var leftFork = forks[index];
                        var rightFork = forks[(index + 1) % forks.Count];

                        var strategy = sp.GetRequiredService<IPhilosopherStrategy>();

                        var philosopher = new PhilosopherService(philosopherNames[index], (uint)index,
                            leftFork, rightFork, strategy, settings.ActionTimes);

                        var philosophers = sp.GetRequiredService<List<MultithreadedPhilosopher>>();
                        philosophers.Add(philosopher);

                        return philosopher;
                    });
                }

                services.AddSingleton<ITableManager, TableManager>();
                services.AddHostedService<DeadlockDetectorService>();
                if (settings.DisplayStatus)
                {
                    services.AddHostedService<StatusPrinterService>();
                }

                services.AddHostedService<TimedShutdownService>();
            });

        return builder.Build();
    }
}