#include "stdafx.h"
#include "ExpOverlapped.h"


ExpOverlapped::ExpOverlapped(COMP_OP Op, const void* const packet)
	: ExpOverlappedBasic{ Op }
{
	Wsabuf.buf = reinterpret_cast<CHAR*>(Buf.data());
	Wsabuf.len = reinterpret_cast<const packet_base<void>*>(packet)->size;
	memcpy(Wsabuf.buf, packet, Wsabuf.len);
}

ExpOverlapped::ExpOverlapped(COMP_OP Op)
	: ExpOverlappedBasic{ Op }
{
	Wsabuf.buf = reinterpret_cast<CHAR*>(Buf.data());
	Wsabuf.len = static_cast<ULONG>(MAX_BUFFER_SIZE);
}

RecvExpOverlapped::RecvExpOverlapped()
	: ExpOverlappedBasic{ COMP_OP::OP_RECV }
{
#ifdef RINGBUFFER
	Wsabuf.buf = reinterpret_cast<CHAR*>(Buf.Begin());
#else
	wsabuf.buf = reinterpret_cast<CHAR*>(buf.data());
#endif // RINGBUFFER
	Wsabuf.len = static_cast<ULONG>(MAX_BUFFER_SIZE);
}