#ifndef THREAD_H_INCLUDED
#define THREAD_H_INCLUDED

#include "search.h"

#include <atomic>
#include <mutex>
#include <thread>
#include <vector>
#include <condition_variable>
#include <unistd.h>


namespace Kojiro{

	class Thread{

		private:
			std::mutex mutex;
			std::condition_variable cv;
			size_t idx;
			bool exit = false, searching = true;
			std::thread stdThread;

		public:
			explicit Thread(size_t);
			virtual ~Thread();
			virtual void search();
			void clear();
			void idle_loop();
			void start_searching();
			void wait_until_search_finished();
			size_t id() const {return idx;}

			std::atomic<uint64_t> nodes;
			int depth;
	};
	
	struct MainThread : public Thread{
		using Thread::Thread;

		void search() override;
		void check_time();

		bool stop_on_ponder_hit;
		std::atomic_bool ponder;
	};

	struct ThreadPool : public std::vector<Thread*>{
		void start_thinking(const Search::GameInfo&, bool, int);
		void clear();
		void set(size_t);

		MainThread* main()			const { return static_cast<MainThread*>(front()); }
		uint64_t nodes_searched() const { return accumulate(&Thread::nodes); }
		Thread* get_best_thread() const;
  		void start_searching();
  		void wait_until_search_finished() const;

		std::atomic_bool stop, increase_depth;

	private:
		uint64_t accumulate(std::atomic<uint64_t> Thread::* member) const {
    		uint64_t sum = 0;
    		for (Thread* th : *this)
        		sum += (th->*member).load(std::memory_order_relaxed);
    		return sum;
  		}
	};

extern ThreadPool Threads;

}

#endif