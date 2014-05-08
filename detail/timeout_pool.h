/**
 * @file:   timeout_pool.h
 * @author: jameyli <jameyli AT tencent DOT com>
 * @brief:  simple timeout pool
 *
 * @date:   2014-04-16
 */

#ifndef TIMEOUT_POOL_H
#define TIMEOUT_POOL_H

#include <stdint.h>
#include <functional>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/indexed_by.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>

namespace tnt
{

class TimeoutPool
{
public:
  typedef int64_t Id;
  // typedef std::function<void(Id, int64_t)> Callback;

  TimeoutPool() : nextId_(1) { }

  Id Add(int64_t now, int64_t delay);

  Id AddRepeating(int64_t now, int64_t interval);

  bool Erase(Id id);

  int64_t NextExpiration() const;


// private:
  int64_t runInternal(int64_t now, bool runOnce);

  struct Event
  {
    Id id;
    int64_t expiration;
    int64_t repeat_interval;
    // Callback callback;
  };

  typedef boost::multi_index_container<
    Event,
    boost::multi_index::indexed_by<
      boost::multi_index::ordered_unique<boost::multi_index::member<
        Event, Id, &Event::id
      > >,
      boost::multi_index::ordered_non_unique<boost::multi_index::member<
        Event, int64_t, &Event::expiration
      > >
    >
  > Set;

  enum
  {
    BY_ID=0,
    BY_EXPIRATION=1
  };

  Set timeouts_;
  Id nextId_;

}; // class TimeoutPool

} // namespace tnt

#endif //TNT_TIMEOUT_POOL_H

