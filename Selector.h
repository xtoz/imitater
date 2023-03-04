#ifndef IMITATER_SELECTOR_H
#define IMITATER_SELECTOR_H

#include "Eventor.h"
#include <map>

namespace imitater
{
class Selector
{
public:
    Selector();
    ~Selector();

    typedef std::vector<Eventor::EventorPtr> EventorPtrList;

    void registerEventor(Eventor::EventorPtr eventor);
    void updateEventor(Eventor::EventorPtr eventor);    // it seems no need in select mode
    void unregisterEventor(Eventor::EventorPtr eventor);

    EventorPtrList select(long timeoutUS);

private:
    std::map<int,Eventor::EventorPtr> _mapEventor;
};
}

#endif