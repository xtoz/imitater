#include "Selector.h"

using namespace imitater;
using namespace std;

Selector::Selector()
{

}

Selector::~Selector()
{

}

void Selector::registerEventor(Eventor::EventorPtr eventor)
{
    if(!_mapEventor.count(eventor->eid()))
    {
        _mapEventor.emplace(pair(eventor->eid(), eventor));
    }
}

void Selector::updateEventor(Eventor::EventorPtr eventor)
{

}

void Selector::unregisterEventor(Eventor::EventorPtr eventor)
{

    if(_mapEventor.count(eventor->eid()))
    {
        _mapEventor.erase(eventor->eid());
    }
}

Selector::EventorPtrList Selector::select(long timeoutUS)
{
    // set events
    fd_set readSet;
    fd_set writeSet;
    FD_ZERO(&readSet);
    FD_ZERO(&writeSet);

    for(auto && fdevt : _mapEventor)
    {
        if(fdevt.second->events() & Eventor::EventRead)
        {
            FD_SET(fdevt.second->sockFD(), &readSet);
        }
        if(fdevt.second->events() & Eventor::EventWrite)
        {
            FD_SET(fdevt.second->sockFD(), &writeSet);
        }
    }

    // select
    timeval timeout = {0, timeoutUS};
    int ret = ::select(0, &readSet, &writeSet, nullptr, &timeout);
    if(0 == ret)
    {
        // TODO:log
    }
    else if(-1 == ret)
    {
        // TODO:error
    }

    // fill active eventor
    EventorPtrList activeList;
    for(auto && fdevt : _mapEventor)
    {
        int revents=Eventor::EventNone;
        if(FD_ISSET(fdevt.second->sockFD(), &readSet))
        {
            revents |= Eventor::EventRead;
        }
        if(FD_ISSET(fdevt.second->sockFD(), &writeSet))
        {
            revents |= Eventor::EventWrite;
        }
        fdevt.second->setRevents(revents);
        if(revents!=Eventor::EventNone)
        {
            activeList.emplace_back(fdevt.second);
        }
    }
    return activeList;
}