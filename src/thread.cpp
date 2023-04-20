#include "thread.h"
#include "bitboard.h"
#include "logger.h"
#include "search.h"

#include <cassert>
#include <cstring>


namespace Kojiro{

ThreadPool Threads;

/// Thread constructor launches the thread and waits until it goes to sleep
/// in idle_loop(). Note that 'searching' and 'exit' should be already set.
Thread::Thread(size_t n) : idx(n), stdThread(&Thread::idle_loop, this){
	wait_until_search_finished();
}

/// Thread destructor wakes up the thread in idle_loop() and waits
/// for its termination. Thread should be already waiting.
Thread::~Thread() {
  assert(!searching);

  exit = true;
  start_searching();
  stdThread.join();
}

/// Thread::clear() reset histories, usually before a new game
void Thread::clear() {
    memset(killer_moves, 0, sizeof(killer_moves));
    memset(history_moves, 0, sizeof(history_moves));
}

/// Thread::start_searching() wakes up the thread that will start the search
void Thread::start_searching() {
  std::lock_guard<std::mutex> lk(mutex);
  searching = true;
  cv.notify_one(); // Wake up the thread in idle_loop()
}

/// Thread::wait_for_search_finished() blocks on the condition variable
/// until the thread has finished searching.
void Thread::wait_until_search_finished() {
  	std::unique_lock<std::mutex> lk(mutex);
  	cv.wait(lk, [&]{ return !searching; });
}

/// Thread::idle_loop() is where the thread is parked, blocked on the
/// condition variable, when it has no work to do.
void Thread::idle_loop() {
  /// @todo what tf it is saying?)
  // If OS already scheduled us on a different group than 0 then don't overwrite
  // the choice, eventually we are one of many one-threaded processes running on
  // some Windows NUMA hardware, for instance in fishtest. To make it simple,
  // just check if running threads are below a threshold, in this case all this
  // NUMA machinery is not needed.
//   if (Options["Threads"] > 8)
//       WinProcGroup::bindThisThread(idx);
  while (true) {
      std::unique_lock<std::mutex> lk(mutex);
      searching = false;
      cv.notify_one(); // Wake up anyone waiting for search finished
      cv.wait(lk, [&]{ return searching; });

      if (exit){
          return;
	  }

        lk.unlock();

        search();
  }
}

/// ThreadPool::set() creates/destroys threads to match the requested number.
/// Created and launched threads will immediately go to sleep in idle_loop.
/// Upon resizing, threads are recreated to allow for binding if necessary.
void ThreadPool::set(size_t requested) {

  if (size() > 0) {   // destroy any existing thread(s)
      main()->wait_until_search_finished();

      while (size() > 0)
          delete back(), pop_back();
  }

  if (requested > 0)   // create new thread(s)
  {
      push_back(new MainThread(0));

      while (size() < requested)
          push_back(new Thread(size()));
      clear();

      // Init thread number dependent search params.
    //   Search::init();
  }
}


/// ThreadPool::clear() sets threadPool data to initial values
void ThreadPool::clear() {
  for (Thread* th : *this)
      th->clear();
}

void ThreadPool::start_thinking(Position& pos, const Search::GameInfo& info, bool ponderMode, int depth){
	main()->wait_until_search_finished();

	main()->stop_on_ponder_hit = stop = false;
	increase_depth = true;
	main()->ponder = ponderMode;
	Search::Info = info;

	for(Thread* th : *this){
    th->nodes = th->alphabeta_nodes = th->quiescence_nodes = 0;
    th->capture_count = th->enpassant_count = th->castle_count = 0;
    th->score = 0;
		th->depth = depth;
    th->scorePV = false;
    th->followPV = true;
    th->rootPos.parse_fen(pos.get_fen(), &th->rootState, th);
    // th->rootState = state;
    memset(th->killer_moves, 0, sizeof(th->killer_moves));
    memset(th->history_moves, 0, sizeof(th->history_moves));
    memset(th->pvLength, 0, sizeof(th->pvLength));
    memset(th->pvTable, 0, sizeof(th->pvTable));
	}

	main()->start_searching();
}

Thread* ThreadPool::get_best_thread() const {
  Thread* bestThread = front();

  for(Thread* th: *this) {
    if (bestThread->score < th->score)
        bestThread = th;
  }

  return bestThread;
}
/// Start non-main threads
void ThreadPool::start_searching() {
    for (Thread* th : *this)
        if (th != front())
            th->start_searching();
}

/// Wait for non-main threads
void ThreadPool::wait_until_search_finished() const {
    for (Thread* th : *this)
        if (th != front())
            th->wait_until_search_finished();
}

}