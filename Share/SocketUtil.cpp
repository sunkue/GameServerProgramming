#include "includes.h"
#include "SocketUtil.h"

////////////////////////////////////////////////////////////////////////////

void SocketUtil::DisplayError(int err, std::wostream& wos)
{
	if (ERROR_IO_PENDING == err)return;

	TCHAR* w_msg = nullptr;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		nullptr, err,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPTSTR>(&w_msg), 0, nullptr);
	MessageBox(nullptr, reinterpret_cast<LPTSTR>(w_msg), nullptr, MB_ICONERROR);
	wos << w_msg << std::endl;
	LocalFree(w_msg);
}

void SocketUtil::terminate()
{
	//DisplayError(WSAGetLastError());
}

void SocketUtil::CheckErrorEx(BOOL ret_val, const char* msg)
{
	if (FALSE == ret_val)
	{
		if (ERROR_IO_PENDING == WSAGetLastError())return;
		ReportError(msg); terminate();
	}
}

void SocketUtil::CheckError(int ret_val, const char* msg)
{
	if (SOCKET_SUCCESS != ret_val)
	{
		if (ERROR_IO_PENDING == WSAGetLastError())return;
		ReportError(msg); terminate();
	}
}

void SocketUtil::CheckError(const SOCKET& Socket_, const char* msg)
{
	if (SOCKET_SUCCESS == Socket_)
	{
		if (ERROR_IO_PENDING == WSAGetLastError())return;
		ReportError(msg); terminate();
	}
}

////////////////////////////////////////////////////////////////////////////