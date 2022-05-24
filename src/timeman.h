#ifndef TIMEMAN_H
#define TIMEMAN_H

#include <chrono>

namespace Kojiro {

typedef std::chrono::milliseconds::rep TimePoint; // A value in milliseconds
static_assert(sizeof(TimePoint) == sizeof(int64_t), "TimePoint should be 64 bits");
inline TimePoint now() {
  return std::chrono::duration_cast<std::chrono::milliseconds>
        (std::chrono::steady_clock::now().time_since_epoch()).count();
}

class TimeManagement {
public:
	void init(int side);
	TimePoint getOptimum() const { return optimumTime; };
	TimePoint getMaximum() const { return maximumTime; };
	TimePoint getElapsed() const { return now() - startTime; };

private:
	TimePoint startTime;
	TimePoint optimumTime;
	TimePoint maximumTime;
};

extern TimeManagement Time;

} // namespace Kojiro

#endif // #ifndef TIMEMAN_H