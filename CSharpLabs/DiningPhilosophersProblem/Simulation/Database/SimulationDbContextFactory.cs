using Microsoft.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore.Design;
using Microsoft.Extensions.Configuration;

namespace Simulation.Database;

public class SimulationDbContextFactory : IDesignTimeDbContextFactory<SimulationDbContext>
{
    public SimulationDbContext CreateDbContext(string[] args)
    {
        var configuration = new ConfigurationBuilder()
            .SetBasePath(Directory.GetCurrentDirectory())
            .AddJsonFile("appsettings.json", optional: false, reloadOnChange: true)
            .Build();

        var connectionString = configuration.GetConnectionString("Default");

        if (string.IsNullOrEmpty(connectionString))
        {
            throw new InvalidOperationException(
                "Connection string 'Default' not found in appsettings.json");
        }

        var optionsBuilder = new DbContextOptionsBuilder<SimulationDbContext>();
        optionsBuilder.UseNpgsql(connectionString);

        return new SimulationDbContext(optionsBuilder.Options);
    }
}