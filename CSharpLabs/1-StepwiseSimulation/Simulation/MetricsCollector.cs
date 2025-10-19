using System.Text;
using Contract;

namespace Simulation;

public class MetricsCollector
{
    private class PhilosopherMetrics
    {
        public int Throughput;
        public int WaitingTime;
    }

    private class ForkMetrics
    {
        public double AvailablePercentage;
        public double TakenPercentage;
        public double InUsePercentage;
    }

    private readonly Dictionary<IPhilosopher, PhilosopherMetrics> _philosopherMetricsMap = new();

    private readonly int _avgThroughput;
    private readonly int _avgWaitingTime;
    private readonly int _maxWaitingTime;
    private readonly IPhilosopher _maxWaitingPhilosopher = null!;

    private readonly Dictionary<IFork, ForkMetrics> _forkMetricsMap = new();

    public MetricsCollector(ITableManager tableManager, int totalTime)
    {
        foreach (var p in tableManager.Philosophers)
        {
            _philosopherMetricsMap.Add(p, new MetricsCollector.PhilosopherMetrics());

            _philosopherMetricsMap[p].Throughput = p.Eaten * 1000 / totalTime;
            _avgThroughput += _philosopherMetricsMap[p].Throughput;

            _philosopherMetricsMap[p].WaitingTime = p.WaitingTime;
            _avgWaitingTime += _philosopherMetricsMap[p].WaitingTime;

            if (_philosopherMetricsMap[p].WaitingTime > _maxWaitingTime)
            {
                _maxWaitingTime = _philosopherMetricsMap[p].WaitingTime;
                _maxWaitingPhilosopher = p;
            }
        }

        _avgThroughput /= tableManager.PhilosopherCount;
        _avgWaitingTime /= tableManager.PhilosopherCount;

        foreach (var f in tableManager.Forks)
        {
            _forkMetricsMap.Add(f, new ForkMetrics());

            _forkMetricsMap[f].AvailablePercentage = f.AvailableTime * 100 / (double) totalTime;
            _forkMetricsMap[f].TakenPercentage = f.TakenTime * 100 / (double) totalTime;
            _forkMetricsMap[f].InUsePercentage = f.InUseTime * 100 / (double) totalTime;
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
        foreach (var kvp in _forkMetricsMap)
        {
            sb.AppendLine($"{kvp.Key.Name}");
            sb.AppendLine($"\tAvailable: {kvp.Value.AvailablePercentage}%");
            sb.AppendLine($"\tTaken: {kvp.Value.TakenPercentage}%");
            sb.AppendLine($"\tInUse: {kvp.Value.InUsePercentage}%");
        }

        return sb.ToString();
    }
}