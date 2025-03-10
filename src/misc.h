#ifndef MISC_H
#define MISC_H

#include <chrono>
#include <iostream>

const std::string version = "0.1.3";

enum SyncCout { IO_LOCK, IO_UNLOCK };
#define sync_cout std::cout << IO_LOCK
#define sync_endl std::endl << IO_UNLOCK

typedef std::chrono::milliseconds::rep TimePoint; // A value in milliseconds
static_assert(sizeof(TimePoint) == sizeof(int64_t), "TimePoint should be 64 bits");
inline TimePoint now() {
	return std::chrono::duration_cast<std::chrono::milliseconds>
        (std::chrono::steady_clock::now().time_since_epoch()).count();
}

#endif // #ifndef MISC_H