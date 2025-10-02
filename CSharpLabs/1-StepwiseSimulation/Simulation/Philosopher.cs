using System.Diagnostics;
using Contract;

namespace Simulation;

public class Philosopher : IPhilosopher
{
    public enum State
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

    private State _state;
    public State CurrentState => _state;
    private int _stepsTillStateChange;

    private int _waitingTime;
    public int WaitingTime => _waitingTime;

    private readonly Fork _leftFork;
    private readonly Fork _rightFork;

    private IStrategy? _strategy;
    private ICoordinator? _coordinator;

    public bool HasLeftFork { get; private set; }
    public bool HasRightFork { get; private set; }

    private int _eaten;
    public int Eaten => _eaten;
    public string Name => _name;
    private readonly String _name;

    public Philosopher(string name, Fork leftFork, Fork rightFork)
    {
        _name = name;
        _leftFork = leftFork;
        _rightFork = rightFork;
        _eaten = 0;

        HasLeftFork = false;
        HasRightFork = false;

        BecomeThinking();
    }
    
    public void SetStrategy(IStrategy strategy)
    {
        _strategy = strategy;
    }

    public void SetCoordinator(ICoordinator coordinator)
    {
       _coordinator = coordinator; 
       _coordinator.DoAction += OnCoordinatorAction;
    }
    
    private void OnCoordinatorAction(IPhilosopher targetPhilosopher, ICoordinator.Action action)
    {
        if (targetPhilosopher != this) return;
        
        switch (action)
        {
            case ICoordinator.Action.TakeLeftFork:
                TakeLeftFork();
                break;
            case ICoordinator.Action.TakeRightFork:
                TakeRightFork();
                break;
            case ICoordinator.Action.ReleaseLeftFork:
                ReleaseLeftFork();
                break;
            case ICoordinator.Action.ReleaseRightFork:
                ReleaseRightFork();
                break;
        }
    }


    private void BecomeThinking()
    {
        Debug.Assert(_stepsTillStateChange <= 0);
        _stepsTillStateChange = random.Next(3, 11);
        _state = State.Thinking;
    }

    private void BecomeEating()
    {
        Debug.Assert(_stepsTillStateChange <= 0);
        _stepsTillStateChange = random.Next(4, 6);
        _state = State.Eating;
        _eaten++;
    }

    private void BecomeHungry()
    {
        Debug.Assert(_stepsTillStateChange <= 0);
        _state = State.Hungry;
    }

    public bool NextStep()
    {
        _stepsTillStateChange--;
        if (_stepsTillStateChange <= 0)
        {
            switch (_state)
            {
                case State.Thinking:
                    BecomeHungry();
                    break;
                case State.Hungry:
                    _waitingTime++;
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
                    _waitingTime++;
                    TakeLeftFork();
                    break;
                case State.WaitingRightFork:
                    _waitingTime++;
                    TakeRightFork();
                    break;
                case State.TakingLeftFork:
                    HasLeftFork = true;
                    _leftFork.Use(this);
                    BecomeHungry();
                    break;
                case State.TakingRightFork:
                    HasRightFork = true;
                    _rightFork.Use(this);
                    BecomeHungry();
                    break;
                case State.Eating:
                    ReleaseLeftFork();
                    ReleaseRightFork();
                    BecomeThinking();
                    break;
            }

            return true; // state changed
        }

        return false; // state unchanged
    }

    public string StateString()
    {
        return $"{_name}: {_state.ToString()} ({1} steps left), съедено: {_eaten}";
    }

    public IFork LeftFork => _leftFork;
    public IFork RightFork => _rightFork;

    public void TakeLeftFork()
    {
        Debug.Assert(_state is State.Hungry or State.WaitingLeftFork);

        if (LeftFork.IsAvailable)
        {
            _state = State.TakingLeftFork;
            _stepsTillStateChange = 2;
            _waitingTime += _stepsTillStateChange;
            _leftFork.Lock(this);
        }
        else
        {
            _state = State.WaitingLeftFork;
            _stepsTillStateChange = 0;
        }
    }

    public void TakeRightFork()
    {
        Debug.Assert(_state is State.Hungry or State.WaitingRightFork);

        if (RightFork.IsAvailable)
        {
            _state = State.TakingRightFork;
            _stepsTillStateChange = 2;
            _waitingTime += _stepsTillStateChange;
            _rightFork.Lock(this);
        }
        else
        {
            _state = State.WaitingRightFork;
            _stepsTillStateChange = 0;
        }
    }

    public void ReleaseLeftFork()
    {
        Debug.Assert(_state is State.Hungry or State.Eating);
        Debug.Assert(!LeftFork.IsAvailable);

        _leftFork.Put();
        HasLeftFork = false;
        _state = State.Hungry;
        _stepsTillStateChange = 0;
    }

    public void ReleaseRightFork()
    {
        Debug.Assert(_state is State.Hungry or State.Eating);
        Debug.Assert(!RightFork.IsAvailable);

        _rightFork.Put();
        HasRightFork = false;
        _state = State.Hungry;
        _stepsTillStateChange = 0;
    }
}