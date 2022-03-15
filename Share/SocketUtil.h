#pragma once


class SocketUtil
{
public:
	constexpr static int SOCKET_SUCCESS = NO_ERROR;

	static void DisplayError(int err, std::wostream& wos = std::wcerr);
	static void terminate();
	static void CheckError(int ret_val);
	static void CheckError(const SOCKET& socket);
};

#define REPORT_ERROR(msg) SocketUtil::ReportErrorWhere(__FILE__, __LINE__, __FUNCTION__, msg);

//////////////////////////////////////

