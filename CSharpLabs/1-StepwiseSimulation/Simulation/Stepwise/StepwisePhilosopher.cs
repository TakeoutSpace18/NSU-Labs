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
    public int WaitingTime { get; private set; }
    public int Eaten { get; private set; }

    private IPhilosopherStrategy? _strategy;
    private IPhilosopherCoordinator? _coordinator;

    public bool HasLeftFork { get; private set; }
    public bool HasRightFork { get; private set; }


    public string Name { get; }


    public StepwisePhilosopher(string name, IFork leftFork, IFork rightFork)
    {
        Name = name;
        LeftFork = leftFork;
        RightFork = rightFork;
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
        LeftFork.Use(this);
        RightFork.Use(this);
        Eaten++;
    }

    private void BecomeHungry()
    {
        Debug.Assert(_stepsTillStateChange <= 0);
        CurrentState = State.Hungry;
    }

    public bool NextStep()
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
                        _strategy.DoAction(this);
                    }
                }

                break;
            case State.WaitingLeftFork:
                WaitingTime++;
                TakeLeftFork();
                break;
            case State.WaitingRightFork:
                WaitingTime++;
                TakeRightFork();
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

    public IFork LeftFork { get; }

    public IFork RightFork { get; }

    public void TakeLeftFork()
    {
        Debug.Assert(CurrentState is State.Hungry or State.WaitingLeftFork);

        if (LeftFork.IsAvailable)
        {
            CurrentState = State.TakingLeftFork;
            _stepsTillStateChange = 2;
            WaitingTime += _stepsTillStateChange;
            LeftFork.Take(this);
        }
        else
        {
            CurrentState = State.WaitingLeftFork;
            _stepsTillStateChange = 0;
        }
    }

    public void TakeRightFork()
    {
        Debug.Assert(CurrentState is State.Hungry or State.WaitingRightFork);

        if (RightFork.IsAvailable)
        {
            CurrentState = State.TakingRightFork;
            _stepsTillStateChange = 2;
            WaitingTime += _stepsTillStateChange;
            RightFork.Take(this);
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
        
        var success = LeftFork.TryTake(this);
        if (!success) return false;
        
        CurrentState = State.TakingLeftFork;
        _stepsTillStateChange = 2;
        WaitingTime += _stepsTillStateChange;

        return true;
    }

    public bool TryTakeRightFork()
    {
        Debug.Assert(CurrentState is State.Hungry or State.WaitingRightFork);
        
        var success = RightFork.TryTake(this);
        if (!success) return false;
        
        CurrentState = State.TakingRightFork;
        _stepsTillStateChange = 2;
        WaitingTime += _stepsTillStateChange;

        return true;
    }

    public void PutLeftFork()
    {
        Debug.Assert(CurrentState is State.Hungry or State.Eating);
        Debug.Assert(!LeftFork.IsAvailable);

        LeftFork.Put();
        HasLeftFork = false;
        CurrentState = State.Hungry;
        _stepsTillStateChange = 0;
    }

    public void PutRightFork()
    {
        Debug.Assert(CurrentState is State.Hungry or State.Eating);
        Debug.Assert(!RightFork.IsAvailable);

        RightFork.Put();
        HasRightFork = false;
        CurrentState = State.Hungry;
        _stepsTillStateChange = 0;
    }
}