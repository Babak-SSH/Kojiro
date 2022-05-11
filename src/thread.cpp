#include <cassert>
#include "thread.h"


namespace Kojiro{

ThreadPool Threads;

/// Thread constructor launches the thread and waits until it goes to sleep
/// in idle_loop(). Note that 'searching' and 'exit' should be already set.
Thread::Thread(size_t n) : idx(n), stdThread(&Thread::idle_loop, this){
	printf("creating thread?\n");
	wait_until_search_finished();
}

/// Thread destructor wakes up the thread in idle_loop() and waits
/// for its termination. Thread should be already waiting.
Thread::~Thread() {
  assert(!searching);
	printf("destroying thread?\n");
  exit = true;
  start_searching();
  stdThread.join();
}


/// Thread::clear() reset histories, usually before a new game
void Thread::clear() {
	printf("clear thread\n");
}

/// Thread::start_searching() wakes up the thread that will start the search
void Thread::start_searching() {
	printf("thread start searching...\n");
  std::lock_guard<std::mutex> lk(mutex);
  searching = true;
  cv.notify_one(); // Wake up the thread in idle_loop()
}

/// Thread::wait_for_search_finished() blocks on the condition variable
/// until the thread has finished searching.
void Thread::wait_until_search_finished() {
	printf("wait until search is finished...\n");
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
	printf("start idle loop...\n");
  while (true)
  {
      std::unique_lock<std::mutex> lk(mutex);
      searching = false;
      cv.notify_one(); // Wake up anyone waiting for search finished
      cv.wait(lk, [&]{ return searching; });

      if (exit){
		  printf("exit idle loop\n");
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

  if (size() > 0)   // destroy any existing thread(s)
  {
      main()->wait_until_search_finished();

      while (size() > 0)
          delete back(), pop_back();
  }

  if (requested > 0)   // create new thread(s)
  {
	  printf("create threads\n");
      push_back(new MainThread(0));

      while (size() < requested)
          push_back(new Thread(size()));
      clear();

      // Reallocate the hash with the new threadpool size
    //   TT.resize(size_t(Options["Hash"]));

      // Init thread number dependent search params.
    //   Search::init();
  }
}


/// ThreadPool::clear() sets threadPool data to initial values

void ThreadPool::clear() {
	printf("clear threadpool data...\n");
  for (Thread* th : *this)
      th->clear();

//   main()->callsCnt = 0;
//   main()->bestPreviousScore = VALUE_INFINITE;
//   main()->bestPreviousAverageScore = VALUE_INFINITE;
//   main()->previousTimeReduction = 1.0;
}

void ThreadPool::start_thinking(bool ponderMode){
	printf("start thinking...\n");
	main()->wait_until_search_finished();

	main()->stop_on_ponder_hit = stop = false;
	increase_depth = true;
	main()->ponder = ponderMode;

	main()->start_searching();
}

/// Start non-main threads

void ThreadPool::start_searching() {
	printf("threadpool start searching\n");
    for (Thread* th : *this)
        if (th != front())
            th->start_searching();
}

/// Wait for non-main threads

void ThreadPool::wait_until_search_finished() const {
	printf("threadpool wait until search is finished\n");
    for (Thread* th : *this)
        if (th != front())
            th->wait_until_search_finished();
}

void Thread::search(){
	printf("searching....\n");
}

void MainThread::search(){
	printf("MainThread searching....\n");
	if(Threads.stop)
		return;
	sleep(4);
	Threads.start_searching();

	Thread::search();

}
}