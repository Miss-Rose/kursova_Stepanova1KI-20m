#include "winsock2.h"
#include <windows.h>
#include <stdio.h>
#include <fstream>

#pragma warning(disable: 4996)

char info[] = "Viberit rezhim roboti:\n1 - Otrimati povidomlennja vid vuzla B\n-----\n2 - Zavershiti robotu vuzla C\n";

void writeMess(char message[]);

void createNamedPipeServer();

int main()
{
	printf(info);

	int command;

	scanf("%d", &command);

	while (command == 1)
	{
		createNamedPipeServer();

		printf(info);
		scanf("%d", &command);
	}

	printf("\n\nVuzol C zavershiv robotu...\n");

	return 0;
}

void writeMess(char message[])
{
	printf("--- --- MESSAGE --- ---\n");
	printf(message);
	printf("\n--- --- --- --- --- ---\n");
}

void createNamedPipeServer()
{
	HANDLE PipeHandle;
	DWORD BytesRead;
	CHAR buffer[256];
	if ((PipeHandle = CreateNamedPipeA("\\\\.\\Pipe\\PipeC", PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE,
		1, 0, 0, 1000, NULL)) == INVALID_HANDLE_VALUE)
	{
		printf_s("CreateNamedPipe failed with error %d\n", GetLastError());
		return;
	}
	if (ConnectNamedPipe(PipeHandle, NULL) == 0)
	{
		printf_s("Error of ConnectNamedPipe ( %d)\n", GetLastError());
		CloseHandle(PipeHandle);
		return;
	}
	if (ReadFile(PipeHandle, buffer, sizeof(buffer), &BytesRead, NULL) <= 0)
	{
		printf_s("ReadFile failed with error %d\n", GetLastError());
		CloseHandle(PipeHandle);
		return;
	}
	buffer[BytesRead] = 0;

	writeMess(buffer);

	if (DisconnectNamedPipe(PipeHandle) == 0)
	{
		printf_s("DisconnectNamedPipe failed with error %d\n",
			GetLastError());
		CloseHandle(PipeHandle);
		return;
	}
	CloseHandle(PipeHandle);
}