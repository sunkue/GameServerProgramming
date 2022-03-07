#pragma once
/*
* This is sunkue's utility hpp. almost JJamBBong. didn't subdivided.
* sunkue@kakao.com //  korean // yoon_sunkue.
*

*/

#include <concepts>
#include <type_traits>
#include <string>
#include <iostream>
#include <string_view>
#include <ranges>
#include <thread>
#include <any>
#include <random>
#include <thread>
#include <chrono>
#include <future>
#include <condition_variable>
#include <queue>
#define MY_NAME_SPACE SUNKUE

#ifndef NONAMESPACE
namespace MY_NAME_SPACE {} using namespace MY_NAME_SPACE; using namespace std;
#endif // NONAMESPACE

#define CASE break; case

/// CODE GENERATE
namespace MY_NAME_SPACE
{

#define DISABLE_COPY(CLASS)								\
        public:											\
            CLASS(const CLASS&) = delete;				\
            CLASS& operator=(const CLASS&) = delete;
	
#define SINGLE_TON(CLASS)								\
		DISABLE_COPY(CLASS)								\
		public:											\
			static CLASS& get()							\
			{											\
				static CLASS _instance;					\
				return _instance;						\
			}											\
		private:										\
			CLASS()

#define GET(var) auto get_##var()const { return var##_; }
#define GET_REF(var) const auto& get_##var()const { return var##_; }
#define GET_REF_UNSAFE(var) auto& get_##var() { return var##_; }
#define SET(var) void set_##var(const auto& value) { var##_ = value; }

#define CREATE_SHARED(RETURN, CLASS)										\
		template<typename ...Arg>											\
		static RETURN create(Arg&&...arg)									\
		{																	\
			struct enabler : public CLASS									\
			{ enabler(Arg&&...arg) :CLASS(std::forward<Arg>(arg)...) {} };	\
			return std::make_shared<enabler>(std::forward<Arg>(arg)...);	\
		}

#define CREATE_UNIQUE(RETURN, CLASS)										\
		template<typename ...Arg>											\
		static RETURN create(Arg&&...arg)									\
		{																	\
			struct enabler : public CLASS									\
			{ enabler(Arg&&...arg) :CLASS(std::forward<Arg>(arg)...) {} };	\
			return std::make_unique<enabler>(std::forward<Arg>(arg)...);	\
		}
}




//	CHRONO
namespace MY_NAME_SPACE
{
	using clk = std::chrono::high_resolution_clock;
	using namespace std::chrono;
	using namespace std::literals::string_view_literals;

	namespace timer {
		class TIMER {
		public:
			static TIMER& get() {
				static TIMER instance_;
				return instance_;
			}

			void start() noexcept {
				_stop_watch = clk::now();
			}

			void end(std::string_view mess = ""sv, std::ostream& os = std::cout) {
				auto t = clk::now();
				_lap = duration_cast<milliseconds>(t - _stop_watch);
				os << mess << " : " << _lap << std::endl;
			}

			const milliseconds get_last_lap() const noexcept { return _lap; }
		private:
			clk::time_point _stop_watch;
			milliseconds _lap{};
		};
	}
}
