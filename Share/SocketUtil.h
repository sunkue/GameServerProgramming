#pragma once


class SocketUtil
{
public:
	constexpr static int SOCKET_SUCCESS = NO_ERROR;

	static void DisplayError(int err, std::wostream& wos = std::wcerr);
	static void terminate();
	static void CheckErrorEx(BOOL ret_val);
	static void CheckError(int ret_val);
	static void CheckError(const SOCKET& socket);
};

//////////////////////////////////////

