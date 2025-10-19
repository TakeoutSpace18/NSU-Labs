using System.Diagnostics;
using Contract;

namespace Simulation.Stepwise;

public class StepwiseFork(string name) : IFork
{
    private enum State
    {
        Available,
        Taken,
        InUse,
    }

    private State _state = State.Available;
    public bool IsAvailable => _state == State.Available;
    public string Name { get; } = name;

    private IPhilosopher? _takenBy = null;

    public int AvailableTime { get; private set; }
    public int TakenTime { get; private set; }
    public int InUseTime { get; private set; }

    public bool TryTake(IPhilosopher philosopher)
    {
        if (_state != State.Available) return false;
        
        Take(philosopher);
        return true;
    }

    public void Take(IPhilosopher philosopher)
    {
        Debug.Assert(_state == State.Available);
        Debug.Assert(_takenBy == null);

        _takenBy = philosopher;
        _state = State.Taken;
    }

    public void Use(IPhilosopher philosopher)
    {
        Debug.Assert(_state == State.Taken);
        Debug.Assert(_takenBy == philosopher);

        _state = State.InUse;
    }

    public void Put()
    {
        Debug.Assert(_takenBy != null);
        Debug.Assert(_state is State.InUse or State.Taken);

        _takenBy = null;
        _state = State.Available;
    }


    public string StateString
    {
        get
        {
            var str = $"{Name}: {_state.ToString()}";
            if (_takenBy != null)
            {
                str += $" (Используется: {_takenBy.Name})";
            }

            return str;
        }
    }

    public bool NextStep()
    {
        switch (_state)
        {
            case State.Available:
                AvailableTime++;
                break;
            case State.Taken:
                TakenTime++;
                break;
            case State.InUse:
                InUseTime++;
                break;
            default:
                throw new ArgumentOutOfRangeException(nameof(_state));
        }

        return false;
    }
}