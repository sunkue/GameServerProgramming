#pragma once

struct SocketUtil
{
	constexpr static int SOCKET_SUCCESS = NO_ERROR;

	static void DisplayError(int err, std::wostream& wos = std::wcerr);
	static void terminate();
	static void CheckErrorEx(BOOL ret_val, const char* msg);
	static void CheckError(int ret_val, const char* msg);
	static void CheckError(const SOCKET& Socket_, const char* msg);
	static void ReportErrorWhere(const char* file_name, int line, auto func, std::string_view msg = "", std::ostream& os = std::cerr)
	{
		os << "[ERROR] " << file_name << " line::" << line << " F::" << func
			<< " err::" << WSAGetLastError() << " msg::" << msg << std::endl;
	}
	static void ReportError(std::string_view msg, std::ostream& os = std::cerr)
	{
		os << "[ERROR] " << msg << " " << WSAGetLastError() << std::endl;
		//SocketUtil::err_display(SocketUtil::GetLastError());
	}
};




//////////////////////////////////////

