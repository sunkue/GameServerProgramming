#pragma once

#include <algorithm>
#include <array>

template<size_t N, class Contanier = std::array<std::byte, N>>
class RingBuffer : protected Contanier
{
	using value_type = Contanier::value_type;
public:
	value_type* Data() { return &Contanier::operator[](BeginIdx_); }
	size_t Size() const { return Size_; }
	size_t Capacity() const { return Contanier::Size(); }
	size_t Write(const void* _src, size_t bytes)
	{
		auto src = reinterpret_cast<const std::byte*>(_src);
		auto Data = Contanier::Data();
		auto Capacity = this->Capacity();
		auto bytes_to_write = std::min(bytes, Capacity - Size_);

		if (bytes_to_write <= Capacity - EndIdx_)
		{
			memcpy(Data + EndIdx_, src, bytes_to_write);
			EndIdx_ += bytes_to_write;
			if (EndIdx_ == Capacity) EndIdx_ = 0;
		}
		else
		{
			auto size_1 = Capacity - EndIdx_;
			memcpy(Data + EndIdx_, src, size_1);
			auto size_2 = bytes_to_write - size_1;
			memcpy(Data, src + size_1, size_2);
			EndIdx_ = size_2;
		}

		Size_ += bytes_to_write;
		return bytes_to_write;
	}
	size_t Read(void* _dst, size_t bytes)
	{
		auto Data = Contanier::Data();
		auto dst = reinterpret_cast<std::byte*>(_dst);
		auto Capacity = this->Capacity();
		auto bytes_to_read = std::min(bytes, Size_);

		if (bytes_to_read <= Capacity - BeginIdx_)
		{
			memcpy(dst, Data + BeginIdx_, bytes_to_read);
			BeginIdx_ += bytes_to_read;
			if (BeginIdx_ == Capacity) BeginIdx_ = 0;
		}
		else
		{
			auto size_1 = Capacity - BeginIdx_;
			memcpy(dst, Data + BeginIdx_, size_1);
			auto size_2 = bytes_to_read - size_1;
			memcpy(dst + size_1, Data, size_2);
			BeginIdx_ = size_2;
		}

		Size_ -= bytes_to_read;
		return bytes_to_read;
	}
private:
	size_t BeginIdx_{}, EndIdx_{}, Size_{};
};



// 멤버함수로는 데이터복사등의 조작이 불가함. 데이터는 내비두고 캐럿만 변화시킴.
template<size_t N, class Contanier = std::array<std::byte, N>>
class RecvRingBuffer : protected Contanier
{
	using value_type = Contanier::value_type;
public:
	value_type* Begin() { return &Contanier::operator[](BeginIdx_); }
	value_type* End() { return &Contanier::operator[](EndIdx_); }
	size_t Size() const { return Size_; }
	size_t Capacity() const { return Contanier::size(); }
	bool Full() const { return Capacity() == Size(); }
	size_t FilledEdgespace() const { return Capacity() - BeginIdx_; }
	size_t EmptyEdgespace() const { return Capacity() - EndIdx_; }
	size_t BytesToRecv() const
	{
		if (Full()) [[unlikely]]
			return 0;

		if (EndIdx_ < BeginIdx_) [[unlikely]]
			return BeginIdx_ - EndIdx_;

		return EmptyEdgespace();
	}
	// write
	void MoveRear(size_t bytes)
	{
		auto Capacity = this->Capacity();
		EndIdx_ = (EndIdx_ + bytes) % Capacity;
		Size_ += bytes;
	}
	// read 
	void MoveFront(size_t bytes)
	{
		auto Capacity = this->Capacity();
		BeginIdx_ = (BeginIdx_ + bytes) % Capacity;
		Size_ -= bytes;
	}

	bool CheckOverflowOnRead(size_t bytes) const
	{
		bool overflowed = FilledEdgespace() < bytes;
		return overflowed;
	}
private:
	size_t BeginIdx_{}, EndIdx_{}, Size_{};
};




