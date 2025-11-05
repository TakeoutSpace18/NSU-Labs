using System.Diagnostics;
using Contract;

namespace Simulation.Stepwise;

public class StepwisePhilosopher : IPhilosopher
{
    private enum State
    {
        Thinking,
        Hungry,
        Eating,
        WaitingLeftFork,
        WaitingRightFork,
        TakingLeftFork,
        TakingRightFork,
    }

    private static readonly Random random = new Random();

    private State CurrentState { get; set; }

    private int _stepsTillStateChange;

    // metrics
    public long WaitingTime { get; private set; }
    public long Eaten { get; private set; }

    private IPhilosopherStrategy? _strategy;
    private IPhilosopherCoordinator? _coordinator;

    public bool HasLeftFork { get; private set; }
    public bool HasRightFork { get; private set; }
    public bool WaitsLeftFork => CurrentState is State.WaitingLeftFork or State.TakingLeftFork;
    public bool WaitsRightFork => CurrentState is State.WaitingRightFork or State.TakingRightFork;

    private readonly StepwiseFork _leftFork;
    private readonly StepwiseFork _rightFork;

    public string Name { get; }
    public uint Id { get; }

    public StepwisePhilosopher(string name, uint id, StepwiseFork leftFork, StepwiseFork rightFork)
    {
        Name = name;
        Id = id;
        
        _leftFork = leftFork;
        _rightFork = rightFork;
        Eaten = 0;

        HasLeftFork = false;
        HasRightFork = false;

        BecomeThinking();
    }
    
    public void SetStrategy(IPhilosopherStrategy philosopherStrategy)
    {
        _strategy = philosopherStrategy;
    }

    public void SetCoordinator(IPhilosopherCoordinator philosopherCoordinator)
    {
       _coordinator = philosopherCoordinator; 
       _coordinator.DoAction += OnCoordinatorAction;
    }
    
    private void OnCoordinatorAction(IPhilosopher targetPhilosopher, IPhilosopherCoordinator.Action action)
    {
        if (targetPhilosopher != this) return;
        
        switch (action)
        {
            case IPhilosopherCoordinator.Action.TakeLeftFork:
                TakeLeftFork();
                break;
            case IPhilosopherCoordinator.Action.TakeRightFork:
                TakeRightFork();
                break;
            case IPhilosopherCoordinator.Action.ReleaseLeftFork:
                PutLeftFork();
                break;
            case IPhilosopherCoordinator.Action.ReleaseRightFork:
                PutRightFork();
                break;
            default:
                throw new ArgumentOutOfRangeException(nameof(action), action, null);
        }
    }


    private void BecomeThinking()
    {
        Debug.Assert(_stepsTillStateChange <= 0);
        _stepsTillStateChange = random.Next(3, 11);
        CurrentState = State.Thinking;
    }

    private void BecomeEating()
    {
        Debug.Assert(_stepsTillStateChange <= 0);
        _stepsTillStateChange = random.Next(4, 6);
        CurrentState = State.Eating;
        _leftFork.Use(this);
        _rightFork.Use(this);
        Eaten++;
    }

    private void BecomeHungry()
    {
        Debug.Assert(_stepsTillStateChange <= 0);
        CurrentState = State.Hungry;
    }

    public bool NextStep(CancellationToken? token = null)
    {
        _stepsTillStateChange--;
        if (_stepsTillStateChange > 0) return false; // state unchanged
        
        switch (CurrentState)
        {
            case State.Thinking:
                BecomeHungry();
                break;
            case State.Hungry:
                WaitingTime++;
                if (HasLeftFork && HasRightFork)
                {
                    BecomeEating();
                }
                else
                {
                    Debug.Assert(_coordinator != null ||  _strategy != null);
                        
                    if (_coordinator != null)
                    {
                        _coordinator.DeclareHungry(this);
                    }
                    else if (_strategy != null)
                    {
                        _strategy.DoAction(this, token);
                    }
                }

                break;
            case State.WaitingLeftFork:
                WaitingTime++;
                TakeLeftFork(token);
                break;
            case State.WaitingRightFork:
                WaitingTime++;
                TakeRightFork(token);
                break;
            case State.TakingLeftFork:
                HasLeftFork = true;
                BecomeHungry();
                break;
            case State.TakingRightFork:
                HasRightFork = true;
                BecomeHungry();
                break;
            case State.Eating:
                PutLeftFork();
                PutRightFork();
                BecomeThinking();
                break;
            default:
                throw new ArgumentOutOfRangeException();
        }

        return true; // state changed
    }

    public string StateString => $"{Name}: {CurrentState.ToString()} ({1} steps left), съедено: {Eaten}";

    public void TakeLeftFork(CancellationToken? token = null)
    {
        Debug.Assert(CurrentState is State.Hungry or State.WaitingLeftFork);

        if (_leftFork.IsAvailable)
        {
            CurrentState = State.TakingLeftFork;
            _stepsTillStateChange = 2;
            WaitingTime += _stepsTillStateChange;
            _leftFork.Take(this);
        }
        else
        {
            CurrentState = State.WaitingLeftFork;
            _stepsTillStateChange = 0;
        }
    }

    public void TakeRightFork(CancellationToken? token = null)
    {
        Debug.Assert(CurrentState is State.Hungry or State.WaitingRightFork);

        if (_rightFork.IsAvailable)
        {
            CurrentState = State.TakingRightFork;
            _stepsTillStateChange = 2;
            WaitingTime += _stepsTillStateChange;
            _rightFork.Take(this);
        }
        else
        {
            CurrentState = State.WaitingRightFork;
            _stepsTillStateChange = 0;
        }
    }

    public bool TryTakeLeftFork()
    {
        Debug.Assert(CurrentState is State.Hungry or State.WaitingLeftFork);
        
        var success = _leftFork.TryTake(this);
        if (!success) return false;
        
        CurrentState = State.TakingLeftFork;
        _stepsTillStateChange = 2;
        WaitingTime += _stepsTillStateChange;

        return true;
    }

    public bool TryTakeRightFork()
    {
        Debug.Assert(CurrentState is State.Hungry or State.WaitingRightFork);
        
        var success = _rightFork.TryTake(this);
        if (!success) return false;
        
        CurrentState = State.TakingRightFork;
        _stepsTillStateChange = 2;
        WaitingTime += _stepsTillStateChange;

        return true;
    }

    public void PutLeftFork()
    {
        Debug.Assert(CurrentState is State.Hungry or State.Eating);
        Debug.Assert(!_leftFork.IsAvailable);

        _leftFork.Put();
        HasLeftFork = false;
        CurrentState = State.Hungry;
        _stepsTillStateChange = 0;
    }

    public void PutRightFork()
    {
        Debug.Assert(CurrentState is State.Hungry or State.Eating);
        Debug.Assert(!_rightFork.IsAvailable);

        _rightFork.Put();
        HasRightFork = false;
        CurrentState = State.Hungry;
        _stepsTillStateChange = 0;
    }
}