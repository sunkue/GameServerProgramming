#include "stdafx.h"
#include "ExpOverlapped.h"


ExpOverlapped::ExpOverlapped(COMP_OP op, const void* const packet)
	: ExpOverlappedBasic{ op }
{
	wsabuf.buf = reinterpret_cast<CHAR*>(buf.data());
	wsabuf.len = reinterpret_cast<const packet_base<void>*>(packet)->size;
	memcpy(wsabuf.buf, packet, wsabuf.len);
}

ExpOverlapped::ExpOverlapped(COMP_OP op)
	: ExpOverlappedBasic{ op }
{
	wsabuf.buf = reinterpret_cast<CHAR*>(buf.data());
	wsabuf.len = static_cast<ULONG>(MAX_BUFFER_SIZE);
}

RecvExpOverlapped::RecvExpOverlapped()
	: ExpOverlappedBasic{ COMP_OP::OP_RECV }
{
#ifdef RINGBUFFER
	wsabuf.buf = reinterpret_cast<CHAR*>(buf.begin());
#else
	wsabuf.buf = reinterpret_cast<CHAR*>(buf.data());
#endif // RINGBUFFER
	wsabuf.len = static_cast<ULONG>(MAX_BUFFER_SIZE);
}