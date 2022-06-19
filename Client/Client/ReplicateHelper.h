#pragma once

#include "Networker.h"

template<class T> constexpr T NeedRequest();
template<> constexpr int NeedRequest() { return -1; }
template<> constexpr float NeedRequest() { return -1; }
template<> constexpr string NeedRequest() { return {}; }

template<class T> constexpr T WaitForRequestAnswer();
template<> constexpr int WaitForRequestAnswer() { return 0; }
template<> constexpr float WaitForRequestAnswer() { return 0; }
template<> constexpr string WaitForRequestAnswer() { return { " " }; }

#define REPLICATE(var) const auto& Get##var()						\
{																	\
	if (var##_ == NeedRequest<decltype(var##_)>())					\
	{																\
		var##_ = WaitForRequestAnswer<decltype(var##_)>();			\
		cs_request_##var request;									\
		request.id = GetId();										\
		Networker::Get().DoSend(&request);							\
	}																\
	return var##_;													\
}


