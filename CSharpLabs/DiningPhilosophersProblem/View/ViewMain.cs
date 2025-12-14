using System.CommandLine;
using System;
using System.CommandLine;
using System.Threading.Tasks;
using Microsoft.EntityFrameworkCore;
using Microsoft.Extensions.Configuration;
using Simulation.Database;

namespace View;

class ViewMain
{
    static async Task<int> Main(string[] args)
    {
        var runIdOption = new Option<Guid>("--runId")
        {
            Description = "The simulation run ID to view",
            Required = true
        };

        var delayOption = new Option<double>(name: "--delay")
        {
            Description = "Time offset in seconds from the start of simulation",
            Required = true
        };

        var rootCommand = new RootCommand("Dining Philosophers State Viewer")
        {
            runIdOption,
            delayOption
        };

        rootCommand.SetAction(async parseResult =>
        {
            await ViewStateAsync(parseResult.GetRequiredValue(runIdOption),
                parseResult.GetRequiredValue(delayOption));
        });

        return await rootCommand.Parse(args).InvokeAsync();
    }

    private static async Task ViewStateAsync(Guid runId, double delaySeconds)
    {
        try
        {
            var configuration = new ConfigurationBuilder()
                .AddJsonFile("appsettings.json", optional: false)
                .Build();

            var connectionString = configuration.GetConnectionString("Default");

            var options = new DbContextOptionsBuilder<SimulationDbContext>()
                .UseNpgsql(connectionString)
                .Options;

            var dbManager = new SimulationDbManager(options);

            var snapshot = await dbManager.GetSnapshotAsync(runId, delaySeconds);

            if (snapshot == null)
            {
                Console.WriteLine($"Run with ID {runId} not found.");
                return;
            }

            Console.WriteLine("=".PadRight(60, '='));
            Console.WriteLine($"Simulation RunId: {snapshot.RunId}");
            Console.WriteLine($"Started at: {snapshot.StartTime:yyyy-MM-dd HH:mm:ss.fff} UTC");
            Console.WriteLine($"Snapshot time: {snapshot.SnapshotTime:yyyy-MM-dd HH:mm:ss.fff} UTC");
            Console.WriteLine($"Time offset: {delaySeconds:F2} seconds");
            Console.WriteLine("=".PadRight(60, '='));
            Console.WriteLine();

            Console.WriteLine("Philosophers:");
            Console.WriteLine("-".PadRight(60, '-'));

            if (snapshot.PhilosopherStates.Count == 0)
            {
                Console.WriteLine("No philosopher states recorded at this time.");
            }
            else
            {
                foreach (var state in snapshot.PhilosopherStates.OrderBy(p => p.Philosopher?.Id))
                {
                    var name = state.Philosopher?.Name ?? "Unknown";
                    Console.WriteLine($"{name,-20} | State: {state.State,-15} | Eaten: {state.Eaten}");
                }
            }

            Console.WriteLine();
            Console.WriteLine("Forks:");
            Console.WriteLine("-".PadRight(60, '-'));

            if (snapshot.ForkStates.Count == 0)
            {
                Console.WriteLine("No fork states recorded at this time.");
            }
            else
            {
                foreach (var state in snapshot.ForkStates.OrderBy(f => f.Fork?.Name))
                {
                    var forkName = state.Fork?.Name ?? "Unknown";
                    var takenInfo = state.TakenByPhilosopher != null
                        ? $" (by {state.TakenByPhilosopher.Name})"
                        : "";
                    Console.WriteLine($"{forkName,-20} | State: {state.State,-15}{takenInfo}");
                }
            }

            Console.WriteLine();
            Console.WriteLine("=".PadRight(60, '='));
        }
        catch (Exception ex)
        {
            Console.WriteLine($"Error: {ex.Message}");
            Console.WriteLine(ex.StackTrace);
        }
    }
}