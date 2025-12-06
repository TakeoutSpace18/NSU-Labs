using Contract;
using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Options;
using Simulation.Multithreaded;

namespace Simulation.GenericHost;

public class PhilosopherService(
    string name,
    uint id,
    MultithreadedFork leftFork,
    MultithreadedFork rightFork,
    IPhilosopherStrategy strategy,
    ActionTimes actionTimes)
    : MultithreadedPhilosopher(name, id, leftFork, rightFork, strategy, actionTimes), IHostedService, IDisposable
{
    private CancellationTokenSource? _stoppingCts;

    private Task? ExecuteTask { get; set; }

    private Task ExecuteAsync(CancellationToken stoppingToken)
    {
        while (!stoppingToken.IsCancellationRequested)
        {
            try
            {
                NextStep(stoppingToken);
            }
            catch (OperationCanceledException)
            {
            }
        }

        return Task.CompletedTask;
    }

    public Task StartAsync(CancellationToken cancellationToken)
    {
        _stoppingCts = CancellationTokenSource.CreateLinkedTokenSource(cancellationToken);
        ExecuteTask = Task.Run(() => ExecuteAsync(_stoppingCts.Token), _stoppingCts.Token);
        return Task.CompletedTask;
    }

    public async Task StopAsync(CancellationToken cancellationToken)
    {
        if (ExecuteTask == null)
        {
            return;
        }

        try
        {
            await _stoppingCts!.CancelAsync();
        }
        finally
        {
            await ExecuteTask.WaitAsync(cancellationToken).ConfigureAwait(ConfigureAwaitOptions.SuppressThrowing);
        }
    }

    public void Dispose()
    {
        _stoppingCts?.Cancel();
    }
}