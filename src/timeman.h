#ifndef TIMEMAN_H
#define TIMEMAN_H

#include "search.h"
#include "misc.h"


namespace Kojiro {

/**
 * @brief time management functions and formulas for calculating optimum
 * and maximum time for each move and total game.
 * 
 */
class TimeManagement {
public:
	void init(Search::GameInfo& info, Color side, int ply);
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