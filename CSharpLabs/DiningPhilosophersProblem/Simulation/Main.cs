using Microsoft.Extensions.Hosting;
using Simulation.GenericHost;

var host = HostSetup.Build(args);
await host.RunAsync();