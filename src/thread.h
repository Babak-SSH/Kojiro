#ifndef THREAD_H_INCLUDED
#define THREAD_H_INCLUDED

#include "position.h"
#include "search.h"
#include "types.h"

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

			std::atomic<uint64_t> nodes, alphabeta_nodes, quiescence_nodes, capture_count, enpassant_count, castle_count;
			int depth;
			StateInfo rootState;
			Position rootPos;
			int killer_moves[12][MaxPly];
			int history_moves[12][64];
			int pvLength[MaxPly];
			int pvTable[MaxPly][MaxPly];
			bool scorePV, followPV;
			int score;
			int bestMove;

	};
	
	struct MainThread : public Thread{
		using Thread::Thread;

		void search() override;
		void check_time();

		bool stop_on_ponder_hit;
		std::atomic_bool ponder;
	};

	struct ThreadPool : public std::vector<Thread*>{
		void start_thinking(Position& pos, const Search::GameInfo&, bool, int);
		void clear();
		void set(size_t);

		MainThread* main()			const { return static_cast<MainThread*>(front()); }
		uint64_t nodes_searched() const { return accumulate(&Thread::nodes); }
  		uint64_t alphabeta_searched() const { return accumulate(&Thread::alphabeta_nodes);}
  		uint64_t quiescence_searched() const { return accumulate(&Thread::quiescence_nodes);}
  		uint64_t captures_searched() const { return accumulate(&Thread::capture_count);}
  		uint64_t enpassants_searched() const { return accumulate(&Thread::enpassant_count);}
  		uint64_t castels_searched() const { return accumulate(&Thread::castle_count);}

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