using Microsoft.Extensions.Configuration;

namespace Simulation;

public abstract class AbstractSimulation
{
    protected readonly SimulationSettings SimulationSettings = new SimulationSettings();
    
    private readonly IConfigurationRoot _config = new ConfigurationBuilder().AddJsonFile("appsettings.json").Build();
    
    protected AbstractSimulation()
    {
        _config.GetSection("SimulationSettings").Bind(SimulationSettings);
    }

    public abstract void Run(int time);
}