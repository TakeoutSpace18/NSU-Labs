using System.Diagnostics;
using Contract;

namespace Simulation;

public class Fork(string name) : IFork
{
    private enum State
    {
        Available,
        Locked,
        InUse,
    }

    private State _state = State.Available;
    public bool IsAvailable => _state == State.Available;
    public string Name { get; } = name;

    private Philosopher? _lockedBy = null;

    public int TotalAvailableSteps { get; private set; }
    public int TotalLockedSteps { get; private set; }
    public int TotalInUseSteps { get; private set; }

    public void Lock(Philosopher philosopher)
    {
        Debug.Assert(_state == State.Available);
        Debug.Assert(_lockedBy == null);

        _lockedBy = philosopher;
        _state = State.Locked;
    }
    public void Use(Philosopher philosopher)
    {
        Debug.Assert(_state == State.Locked);
        Debug.Assert(_lockedBy == philosopher);

        _state = State.InUse;
    }

    public void Put()
    {
        Debug.Assert(_lockedBy != null);
        Debug.Assert(_state is State.InUse or State.Locked);
        
        _lockedBy = null;
        _state = State.Available;
    }

    public string StateString()
    {
        string str = $"{Name}: {_state.ToString()}";
        if (_lockedBy != null)
        {
            str += $" (Используется: {_lockedBy.Name})";
        }

        return str;
    }

    public void NextStep()
    {
        switch (_state)
        {
            case State.Available:
                TotalAvailableSteps++;
                break;
            case State.Locked:
                TotalLockedSteps++;
                break;
            case State.InUse:
                TotalInUseSteps++;
                break;
        }
    }

}