#pragma once
/*
*/

#include <string>
#include <iostream>
#include <string_view>
#include <chrono>
#include <map>
#include <functional>

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
}
