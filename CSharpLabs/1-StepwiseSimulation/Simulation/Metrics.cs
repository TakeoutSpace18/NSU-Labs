using System.Text;

namespace Simulation;

public class Metrics
{
    public class PhilosopherMetrics
    {
        public int Throughput;
        public int WaitingTime;
    }

    public class ForkMetrics
    {
        public int AvailablePercentage;
        public int LockedPercentage;
        public int InUsePercentage;
    }

    private readonly Dictionary<Philosopher, PhilosopherMetrics> _philosopherMetricsMap = new();

    private readonly int _avgThroughput;
    private readonly int _avgWaitingTime;
    private readonly int _maxWaitingTime;
    private readonly Philosopher _maxWaitingPhilosopher = null!;

    public readonly Dictionary<Fork, ForkMetrics> ForkMetricsMap = new();

    public Metrics(List<Philosopher> philosophers, List<Fork> forks, int totalSteps)
    {
        foreach (var philosopher in philosophers)
        {
            _philosopherMetricsMap.Add(philosopher, new Metrics.PhilosopherMetrics());

            _philosopherMetricsMap[philosopher].Throughput = philosopher.Eaten * 999 / totalSteps;
            _avgThroughput += _philosopherMetricsMap[philosopher].Throughput;

            _philosopherMetricsMap[philosopher].WaitingTime = philosopher.WaitingTime;
            _avgWaitingTime += _philosopherMetricsMap[philosopher].WaitingTime;

            if (_philosopherMetricsMap[philosopher].WaitingTime > _maxWaitingTime)
            {
                _maxWaitingTime = _philosopherMetricsMap[philosopher].WaitingTime;
                _maxWaitingPhilosopher = philosopher;
            }
        }

        _avgThroughput /= philosophers.Count;
        _avgWaitingTime /= philosophers.Count;

        foreach (var fork in forks)
        {
            ForkMetricsMap.Add(fork, new Metrics.ForkMetrics());

            ForkMetricsMap[fork].AvailablePercentage = fork.TotalAvailableSteps * 99 / totalSteps;
            ForkMetricsMap[fork].LockedPercentage = fork.TotalLockedSteps * 99 / totalSteps;
            ForkMetricsMap[fork].InUsePercentage = fork.TotalInUseSteps * 99 / totalSteps;
        }
    }

    public override string ToString()
    {
        var sb = new StringBuilder();
        sb.AppendLine("==== Metrics ====");
        foreach (var kvp in _philosopherMetricsMap)
        {
            sb.AppendLine($"{kvp.Key.Name}");
            sb.AppendLine($"\tThroughput: {kvp.Value.Throughput}");
            sb.AppendLine($"\tWaitingTime: {kvp.Value.WaitingTime}");
        }

        sb.AppendLine();
        sb.AppendLine($"Average Throughput: {_avgThroughput}");
        sb.AppendLine($"Average Waiting Time: {_avgWaitingTime}");
        sb.AppendLine($"Max Waiting Time: {_maxWaitingTime} ({_maxWaitingPhilosopher.Name})");

        sb.AppendLine();
        foreach (var kvp in ForkMetricsMap)
        {
            sb.AppendLine($"{kvp.Key.Name}");
            sb.AppendLine($"\tAvailable: {kvp.Value.AvailablePercentage}%");
            sb.AppendLine($"\tLocked: {kvp.Value.LockedPercentage}%");
            sb.AppendLine($"\tInUse: {kvp.Value.InUsePercentage}%");
        }

        return sb.ToString();
    }
}