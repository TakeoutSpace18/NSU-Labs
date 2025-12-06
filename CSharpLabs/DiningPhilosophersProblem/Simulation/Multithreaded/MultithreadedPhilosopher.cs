using System.Diagnostics;
using Contract;

namespace Simulation.Multithreaded;

public class MultithreadedPhilosopher : IPhilosopher
{
    public enum State
    {
        Thinking,
        Hungry,
        Eating,
        TakingLeftFork,
        TakingRightFork,
    }

    private static readonly Random random = new Random();
    private readonly Stopwatch _stopwatch = new Stopwatch();

    private readonly ActionTimes _actionTimes;

    private int _currentState;

    public State CurrentState
    {
        get => (State)Interlocked.CompareExchange(ref _currentState, 0, 0);
        set => Interlocked.Exchange(ref _currentState, (int)value);
    }

    // metrics
    private long _waitingTime;
    private long _eaten;
    public long WaitingTime => Interlocked.Read(ref _waitingTime);
    public long Eaten => Interlocked.Read(ref _eaten);

    private readonly IPhilosopherStrategy? _strategy;

    private bool _hasLeftFork, _hasRightFork, _waitsLeftFork, _waitsRightFork;

    public bool HasLeftFork
    {
        get => Interlocked.CompareExchange(ref _hasLeftFork, false, false);
        private set => Interlocked.Exchange(ref _hasLeftFork, value);
    }

    public bool HasRightFork
    {
        get => Interlocked.CompareExchange(ref _hasRightFork, false, false);
        private set => Interlocked.Exchange(ref _hasRightFork, value);
    }

    public bool WaitsLeftFork
    {
        get => Interlocked.CompareExchange(ref _waitsLeftFork, false, false);
        private set => Interlocked.Exchange(ref _waitsLeftFork, value);
    }

    public bool WaitsRightFork
    {
        get => Interlocked.CompareExchange(ref _waitsRightFork, false, false);
        private set => Interlocked.Exchange(ref _waitsRightFork, value);
    }

    private readonly MultithreadedFork _leftFork;
    private readonly MultithreadedFork _rightFork;

    public string Name { get; }
    public uint Id { get; }

    public MultithreadedPhilosopher(string name, uint id, MultithreadedFork leftFork, MultithreadedFork rightFork,
        IPhilosopherStrategy strategy, ActionTimes actionTimes)
    {
        _actionTimes = actionTimes;

        Name = name;
        Id = id;
        _leftFork = leftFork;
        _rightFork = rightFork;

        HasLeftFork = false;
        HasRightFork = false;

        _strategy = strategy;

        CurrentState = State.Thinking;
    }

    public bool NextStep(CancellationToken? token = null)
    {
        switch (CurrentState)
        {
            case State.Thinking:
                Task.Delay(random.Next(_actionTimes.ThinkingMinMs, _actionTimes.ThinkingMaxMs)).Wait();
                _stopwatch.Restart();
                CurrentState = State.Hungry;
                return true;
            
            case State.Hungry:
                if (HasLeftFork && HasRightFork)
                {
                    CurrentState = State.Eating;
                    _leftFork.Use(this);
                    _rightFork.Use(this);

                    Interlocked.Increment(ref _eaten);
                }
                else
                {
                    Debug.Assert(_strategy != null);

                    _strategy?.DoAction(this, token);
                }

                break;
            case State.TakingLeftFork:
                Task.Delay(_actionTimes.ForkAcquisitionMs).Wait();
                HasLeftFork = true;
                WaitsLeftFork = false;
                CurrentState = State.Hungry;
                break;
            case State.TakingRightFork:
                Task.Delay(_actionTimes.ForkAcquisitionMs).Wait();
                HasRightFork = true;
                WaitsRightFork = false;
                CurrentState = State.Hungry;
                break;
            case State.Eating:
                Interlocked.Add(ref _waitingTime, _stopwatch.ElapsedMilliseconds);
                Task.Delay(random.Next(_actionTimes.EatingMinMs, _actionTimes.EatingMaxMs)).Wait();
                PutLeftFork();
                PutRightFork();
                CurrentState = State.Thinking;
                return true;
            default:
                throw new ArgumentOutOfRangeException();
        }

        return true; // state changed
    }

    public string StateString => $"{Name}: {CurrentState.ToString()}, съедено: {Eaten}";

    public void TakeLeftFork(CancellationToken? token = null)
    {
        Debug.Assert(CurrentState is State.Hungry);

        WaitsLeftFork = true;
        _leftFork.Take(this, token);

        CurrentState = State.TakingLeftFork;
    }

    public void TakeRightFork(CancellationToken? token = null)
    {
        Debug.Assert(CurrentState is State.Hungry);

        WaitsRightFork = true;
        _rightFork.Take(this, token);

        WaitsRightFork = false;
        CurrentState = State.TakingRightFork;
    }

    public bool TryTakeLeftFork()
    {
        Debug.Assert(CurrentState is State.Hungry);

        var success = _leftFork.TryTake(this);
        if (!success) return false;

        CurrentState = State.TakingLeftFork;
        return true;
    }

    public bool TryTakeRightFork()
    {
        Debug.Assert(CurrentState is State.Hungry);

        var success = _rightFork.TryTake(this);
        if (!success) return false;

        CurrentState = State.TakingRightFork;
        return true;
    }

    public void PutLeftFork()
    {
        Debug.Assert(CurrentState is State.Hungry or State.Eating);

        _leftFork.Put();
        HasLeftFork = false;
        CurrentState = State.Hungry;
    }

    public void PutRightFork()
    {
        Debug.Assert(CurrentState is State.Hungry or State.Eating);

        _rightFork.Put();
        HasRightFork = false;
        CurrentState = State.Hungry;
    }
}