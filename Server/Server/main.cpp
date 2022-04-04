#include "stdafx.h"
#include "Server.h"

int main()
{
	Server::get().ProcessQueuedCompleteOperationLoop();
}