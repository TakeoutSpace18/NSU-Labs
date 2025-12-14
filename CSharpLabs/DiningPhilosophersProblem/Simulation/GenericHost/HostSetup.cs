using Contract;
using Microsoft.EntityFrameworkCore;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Simulation.Database;
using Simulation.Multithreaded;
using StrategyImpl;

namespace Simulation.GenericHost;

public static class HostSetup
{
    public static IHost Build(string[]? args)
    {
        var builder = Host.CreateDefaultBuilder(args)
            .ConfigureAppConfiguration(ConfigureAppSettings)
            .ConfigureServices(ConfigureServices);
        
        return builder.Build();
    }

    private static void ConfigureAppSettings(HostBuilderContext ctx, IConfigurationBuilder conf)
    {
        conf.AddJsonFile("appsettings.json", optional: true, reloadOnChange: true);
    }

    private static void ConfigureServices(HostBuilderContext ctx, IServiceCollection services)
    {
        var settings = LoadSimulationSettings(ctx, services);
        var connectionString = ctx.Configuration.GetConnectionString("Default");
        
        ConfigureDatabase(services, connectionString);
        RegisterPhilosopherStrategy(services, settings);
        RegisterForks(services, settings);
        RegisterPhilosophers(services, settings, connectionString);
        RegisterMonitoringServices(services, settings);
    }

    private static SimulationSettings LoadSimulationSettings(
        HostBuilderContext ctx, 
        IServiceCollection services)
    {
        var settings = ctx.Configuration.GetSection("SimulationSettings").Get<SimulationSettings>() 
            ?? throw new ArgumentNullException();
        
        services.Configure<SimulationSettings>(ctx.Configuration.GetSection("SimulationSettings"));
        
        return settings;
    }

    private static void ConfigureDatabase(IServiceCollection services, string? connectionString)
    {
        services.AddDbContext<SimulationDbContext>(options =>
            options.UseNpgsql(connectionString));
        
        services.AddSingleton<SimulationDbManager>(sp =>
        {
            var scopeFactory = sp.GetRequiredService<IServiceScopeFactory>();
            using var scope = scopeFactory.CreateScope();
            var context = scope.ServiceProvider.GetRequiredService<SimulationDbContext>();
            context.Database.Migrate();
            
            var dbContextOptions = new DbContextOptionsBuilder<SimulationDbContext>()
                .UseNpgsql(connectionString)
                .Options;
            
            return new SimulationDbManager(dbContextOptions);
        });
    }

    private static void RegisterPhilosopherStrategy(
        IServiceCollection services, 
        SimulationSettings settings)
    {
        services.AddSingleton<IPhilosopherStrategy>(sp =>
            PhilosopherStrategyFactory.Create(settings.PhilosopherStrategy));
    }

    private static void RegisterForks(IServiceCollection services, SimulationSettings settings)
    {
        services.AddSingleton(sp =>
        {
            var dbManager = sp.GetRequiredService<SimulationDbManager>();
            var forks = new List<MultithreadedFork>();
            
            for (var i = 0; i < settings.PhilosopherNames.Count; i++)
            {
                forks.Add(new MultithreadedFork($"Fork-{i}", (uint)i, dbManager));
            }
            
            return forks;
        });
    }

    private static void RegisterPhilosophers(
        IServiceCollection services, 
        SimulationSettings settings,
        string? connectionString)
    {
        var philosopherNames = settings.PhilosopherNames;
        
        services.AddSingleton<List<MultithreadedPhilosopher>>();
        
        for (var i = 0; i < philosopherNames.Count; i++)
        {
            var index = i;
            services.AddSingleton<IHostedService>(sp =>
            {
                var dbManager = sp.GetRequiredService<SimulationDbManager>();
                var forks = sp.GetRequiredService<List<MultithreadedFork>>();
                var leftFork = forks[index];
                var rightFork = forks[(index + 1) % forks.Count];
                var strategy = sp.GetRequiredService<IPhilosopherStrategy>();
                
                var philosopher = new PhilosopherService(
                    philosopherNames[index],
                    (uint)index,
                    leftFork,
                    rightFork,
                    strategy,
                    settings.ActionTimes,
                    dbManager);
                
                var philosophers = sp.GetRequiredService<List<MultithreadedPhilosopher>>();
                philosophers.Add(philosopher);
                
                return philosopher;
            });
        }
    }

    private static void RegisterMonitoringServices(
        IServiceCollection services, 
        SimulationSettings settings)
    {
        services.AddSingleton<ITableManager, TableManager>();
        services.AddHostedService<DeadlockDetectorService>();
        
        if (settings.DisplayStatus)
        {
            services.AddHostedService<StatusPrinterService>();
        }
        
        services.AddHostedService<TimedShutdownService>();
    }
}