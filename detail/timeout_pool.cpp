/**
 * @file:   timeout_pool.cpp
 * @author: jameyli <jameyli AT tencent DOT com>
 * @brief:  simple timeout pool
 *
 * @date:   2014-04-16
 */

#include "timeout_pool.h"
#include <algorithm>
#include <vector>
#include <utility>
#include <iostream>

namespace tnt
{

TimeoutPool::Id TimeoutPool::Add(int64_t now, int64_t delay)
{
  Id id = nextId_++;
  // Event e;
  timeouts_.insert({id, now + delay, -1});
  // timeouts_.insert(e);
  return id;
}

TimeoutPool::Id TimeoutPool::AddRepeating(int64_t now, int64_t interval)
{
    Id id = nextId_++;
    // Event e;
    timeouts_.insert({id, now + interval, interval});

    return id;
}

// TimeoutPool::Id TimeoutPool::addRepeating(int64_t now, int64_t interval);

bool TimeoutPool::Erase(Id id)
{
    return timeouts_.get<BY_ID>().erase(id);
}

int64_t TimeoutPool::NextExpiration() const
{
  return (timeouts_.empty() ? -1:
          timeouts_.get<BY_EXPIRATION>().begin()->expiration);
}

int64_t TimeoutPool::runInternal(int64_t now, bool runOnce)
{
    // Set::index<Id>::type& byExpiration = timeouts_.get<BY_EXPIRATION>();
    auto& byExpiration = timeouts_.get<BY_EXPIRATION>();
    int64_t nextExp;

    do
    {
        auto end = byExpiration.upper_bound(now);
        std::vector<Event> expired;
        // std::move(byExpiration.begin(), end, std::back_inserter(expired));
        byExpiration.erase(byExpiration.begin(), end);
        // for (auto& event : expired) {
        for (size_t i=0; i<expired.size(); ++i) {
            Event& event = expired[i];
            // Reinsert if repeating, do this before executing callbacks
            // so the callbacks have a chance to call erase
            if (event.repeat_interval >= 0) {
                timeouts_.insert({event.id, now + event.repeat_interval, event.repeat_interval});
            }
        }

        // Call callbacks
        // for (auto& event : expired) {
        for (size_t i=0; i<expired.size(); ++i) {
            Event& event = expired[i];
            // event.callback(event.id, now);
            std::cout<<event.id;
        }

        nextExp = NextExpiration();
    } while (!runOnce && nextExp <= now);
    return nextExp;
}

}

