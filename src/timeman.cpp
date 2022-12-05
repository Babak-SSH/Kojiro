#include "timeman.h"
#include "search.h"

#include <cmath>


namespace Kojiro {
	TimeManagement Time; // main TimeMangement object

	void TimeManagement::init(Search::GameInfo& info, Color side, int ply) {
		startTime = info.startTime;
		int moveOverHead = 10;
		int remainingMoves = 50;	
		double optScale;

		TimePoint timeLeft = std::max(TimePoint(1), info.time[side] + (info.inc[side] * (remainingMoves-1)) - (moveOverHead * remainingMoves));
		
		optScale = std::min(0.0088 + std::pow(ply + 3.0, 0.5) * 0.0044,
							0.88 * info.time[st->side] / double(timeLeft));

		// for move limit
		// optScale = std::min((0.88 + ply / 116.4) / remainingMoves,
                            // 0.88 * info.time[st->side] / double(timeLeft));

		optimumTime = TimePoint(optScale * timeLeft);
		maximumTime = TimePoint(timeLeft);
	}
}