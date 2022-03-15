#include "stdafx.h"
#include "Server.h"

int main()
{
	Server::get().accept();
	Server::get().do_recv();
}