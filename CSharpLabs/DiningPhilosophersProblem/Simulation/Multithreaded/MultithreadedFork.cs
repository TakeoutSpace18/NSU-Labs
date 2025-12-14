using System.Diagnostics;
using Contract;
using Simulation.Database;

namespace Simulation.Multithreaded;

public class MultithreadedFork : IFork
{
    public enum State
    {
        Available,
        Taken,
        InUse,
    }

    private readonly SemaphoreSlim _takenSemaphore = new(1, 1);
    private readonly Stopwatch _stopwatch = new();

    public State CurrentState
    {
        get => _currentState;
        private set
        {
            _currentState = value;
            _ = LogStateChangeAsync(CurrentState);
        }
    }

    public string Name { get; }
    public uint Id { get; }

    private IPhilosopher? _takenBy;

    private long _availableTime;
    private long _takenTime;
    private long _inUseTime;
    private State _currentState;
    private readonly SimulationDbManager? _simulationDbManager;
    public long AvailableTime => Interlocked.Read(ref _availableTime);
    public long TakenTime => Interlocked.Read(ref _takenTime);
    public long InUseTime => Interlocked.Read(ref _inUseTime);

    public MultithreadedFork(string name, uint id, SimulationDbManager? simulationDbManager)
    {
        Name = name;
        Id = id;
        _simulationDbManager = simulationDbManager;
        _stopwatch.Start();
        
        _simulationDbManager?.RegisterForkAsync(Id, Name).Wait();
        
        CurrentState = State.Available;
    }


    private async Task LogStateChangeAsync(State newState)
    {
        try
        {
            if (_simulationDbManager != null)
                await _simulationDbManager.LogForkStateAsync(Id, newState.ToString(), _takenBy?.Id, DateTime.UtcNow);
        }
        catch (Exception ex)
        {
            Console.WriteLine($"Error logging state for {Name}: {ex.Message}");
        }
    }

    public bool TryTake(IPhilosopher philosopher)
    {
        if (!_takenSemaphore.Wait(0)) return false;

        TakeImpl(philosopher);
        return true;
    }

    public void Take(IPhilosopher philosopher, CancellationToken? token = null)
    {
        _takenSemaphore.Wait(token ?? CancellationToken.None);
        TakeImpl(philosopher);
    }

    private void TakeImpl(IPhilosopher philosopher)
    {
        Debug.Assert(_takenSemaphore.CurrentCount == 0);
        Debug.Assert(CurrentState == State.Available);
        Debug.Assert(_takenBy == null);

        _takenBy = philosopher;
        CurrentState = State.Taken;

        Interlocked.Add(ref _availableTime, _stopwatch.ElapsedMilliseconds);
        _stopwatch.Restart();
    }

    public void Use(IPhilosopher philosopher)
    {
        Debug.Assert(_takenSemaphore.CurrentCount == 0);
        Debug.Assert(CurrentState == State.Taken);
        Debug.Assert(_takenBy == philosopher);

        CurrentState = State.InUse;

        Interlocked.Add(ref _takenTime, _stopwatch.ElapsedMilliseconds);
        _stopwatch.Restart();
    }

    public void Put()
    {
        Debug.Assert(_takenSemaphore.CurrentCount == 0);
        Debug.Assert(_takenBy != null);
        Debug.Assert(CurrentState is State.InUse or State.Taken);

        switch (CurrentState)
        {
            case State.Taken:
                Interlocked.Add(ref _takenTime, _stopwatch.ElapsedMilliseconds);
                break;
            case State.InUse:
                Interlocked.Add(ref _inUseTime, _stopwatch.ElapsedMilliseconds);
                break;
            case State.Available:
            default:
                throw new ArgumentOutOfRangeException();
        }

        _stopwatch.Restart();

        _takenBy = null;
        CurrentState = State.Available;

        _takenSemaphore.Release();
    }


    public string StateString
    {
        get
        {
            var str = $"{Name}: {CurrentState.ToString()}";
            if (_takenBy != null)
            {
                str += $" (Используется: {_takenBy.Name})";
            }

            return str;
        }
    }

    public bool NextStep()
    {
        // do nothing
        return false;
    }
}