#ifndef IMITATER_STATEMACHINE_H
#define IMITATER_STATEMACHINE_H

namespace imitater
{
template<class T>
class StateMachine
{
protected:
    StateMachine(T state) {_state = state;}
    T _state;
    virtual bool turnTo(T state) = 0;
};
}

#endif