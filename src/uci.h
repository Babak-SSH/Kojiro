#ifndef UCI_H
#define UCI_H

#include <iostream>
#include <cstdio>
#include <mutex>
#include <sstream>

#include "position.h"
#include "movegen.h"


enum SyncCout { IO_LOCK, IO_UNLOCK };
#define sync_cout std::cout << IO_LOCK
#define sync_endl std::endl << IO_UNLOCK

/// Used to serialize access to std::cout to avoid multiple threads writing at
/// the same time.

inline std::ostream& operator<<(std::ostream& os, SyncCout sc) {

  static std::mutex m;

  if (sc == IO_LOCK)
      m.lock();

  if (sc == IO_UNLOCK)
      m.unlock();

  return os;
}

namespace UCI{
	void loop(int argc, char* argv[]);
	void go(std::istringstream& iss);
	void position(std::istringstream& iss);
	void move();
	int parse_move(std::string m);
}
#endif