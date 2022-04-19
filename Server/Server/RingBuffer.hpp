#pragma once

#include <algorithm>
#include <array>

template<size_t N, class Contanier = std::array<std::byte, N>>
class RingBuffer : protected Contanier
{
	using value_type = Contanier::value_type;
public:
	value_type* data() { return &Contanier::operator[](begin_idx_); }
	size_t size() const { return size_; }
	size_t capacity() const { return Contanier::size(); }
	size_t write(const void* _src, size_t bytes)
	{
		auto src = reinterpret_cast<const std::byte*>(_src);
		auto data = Contanier::data();
		auto capacity = this->capacity();
		auto bytes_to_write = std::min(bytes, capacity - size_);

		if (bytes_to_write <= capacity - end_idx_)
		{
			memcpy(data + end_idx_, src, bytes_to_write);
			end_idx_ += bytes_to_write;
			if (end_idx_ == capacity) end_idx_ = 0;
		}
		else
		{
			auto size_1 = capacity - end_idx_;
			memcpy(data + end_idx_, src, size_1);
			auto size_2 = bytes_to_write - size_1;
			memcpy(data, src + size_1, size_2);
			end_idx_ = size_2;
		}

		size_ += bytes_to_write;
		return bytes_to_write;
	}
	size_t read(void* _dst, size_t bytes)
	{
		auto data = Contanier::data();
		auto dst = reinterpret_cast<std::byte*>(_dst);
		auto capacity = this->capacity();
		auto bytes_to_read = std::min(bytes, size_);

		if (bytes_to_read <= capacity - begin_idx_)
		{
			memcpy(dst, data + begin_idx_, bytes_to_read);
			begin_idx_ += bytes_to_read;
			if (begin_idx_ == capacity) begin_idx_ = 0;
		}
		else
		{
			auto size_1 = capacity - begin_idx_;
			memcpy(dst, data + begin_idx_, size_1);
			auto size_2 = bytes_to_read - size_1;
			memcpy(dst + size_1, data, size_2);
			begin_idx_ = size_2;
		}

		size_ -= bytes_to_read;
		return bytes_to_read;
	}
private:
	size_t begin_idx_{}, end_idx_{}, size_{};
};



// ����Լ��δ� �����ͺ������ ������ �Ұ���. �����ʹ� ����ΰ� ĳ���� ��ȭ��Ŵ.
template<size_t N, class Contanier = std::array<std::byte, N>>
class RecvRingBuffer : protected Contanier
{
	using value_type = Contanier::value_type;
public:
	value_type* begin() { return &Contanier::operator[](begin_idx_); }
	value_type* end() { return &Contanier::operator[](end_idx_); }
	size_t size() const { return size_; }
	size_t capacity() const { return Contanier::size(); }
	bool full() const { return capacity() == size(); }
	size_t filled_edgespace() const { return capacity() - begin_idx_; }
	size_t empty_edgespace() const { return capacity() - end_idx_; }
	size_t bytes_to_recv() const
	{
		if (full()) [[unlikely]]
			return 0;

		if (end_idx_ < begin_idx_) [[unlikely]]
			return begin_idx_ - end_idx_;

		return empty_edgespace();
	}
	// write
	void move_rear(size_t bytes)
	{
		auto capacity = this->capacity();
		end_idx_ = (end_idx_ + bytes) % capacity;
		size_ += bytes;
	}
	// read :: return true when overflowed
	void move_front(size_t bytes)
	{
		auto capacity = this->capacity();
		begin_idx_ = (begin_idx_ + bytes) % capacity;
		size_ -= bytes;
	}

	bool check_overflow_when_read(size_t bytes) const
	{
		bool overflowed = filled_edgespace() < bytes;
		return overflowed;
	}
private:
	size_t begin_idx_{}, end_idx_{}, size_{};
};