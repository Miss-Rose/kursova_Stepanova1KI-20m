#include "winsock2.h"
#include <windows.h>
#include <stdio.h>
#include <fstream>

#pragma warning(disable: 4996)
#define PORT_AB 4444
#define PORT_BA 5555
#define BUFFLEN 4096
#define PIPE_NAME "\\\\.\\Pipe\\PipeC"

char info[] = "Viberit rezhim roboti:\n1 - Vidpraviti povidomlennja na vuzol A\n2 - Vidpraviti povidomlennja na vuzol C\n3 - Otrimati povidomlennja vid vuzla A\n4 - Otrimati povidomlennja vid vuzla D\n-----\n5 - Zavershiti robotu vuzla B\n";
char IPAddr[] = "127.0.0.1";

void readMess();
void writeMess(char message[]);

void createWinSockClient();
void createNamedPipeClient();
void createWinSockServer();
void createNamedPipeServer();

char* message = new char[4096];
int messLen = 0;


int main()
{
	printf(info);

	int command;

	scanf("%d", &command);

	while (command >= 1 && command <= 4)
	{
		switch (command)
		{
		case 1:
			createWinSockClient();
			break;
		case 2:
			createNamedPipeClient();
			break;
		case 3:
			createWinSockServer();
			break;
		case 4:
			createNamedPipeServer();
			break;
		default:
			break;
		}

		printf(info);
		scanf("%d", &command);
	}

	printf("\n\nVuzol B zavershiv robotu...\n");

	return 0;
}

void readMess()
{
	char mess[4096];

	std::ifstream inf("input.txt");

	inf >> mess;

	message = strcpy(message, mess);
	messLen = strlen(message);
}

void writeMess(char message[])
{
	std::ofstream out;
	out.open("output.txt");
	if (out.is_open())
	{
		out << message;
		out << "\n";
	}
}

void createWinSockClient()
{
	WSADATA wsd;
	SOCKET s;
	sockaddr_in SenderAddr, ReceiverAddr;
	WSAStartup(0x0202, &wsd);
	s = socket(AF_INET, SOCK_DGRAM, 0);

	SenderAddr.sin_addr.S_un.S_addr = inet_addr(IPAddr);
	SenderAddr.sin_port = htons(PORT_BA);
	SenderAddr.sin_family = AF_INET;
	ReceiverAddr.sin_addr.S_un.S_addr = inet_addr(IPAddr);
	ReceiverAddr.sin_port = htons(PORT_BA);
	ReceiverAddr.sin_family = AF_INET;

	bind(s, (sockaddr*)&SenderAddr, sizeof(SenderAddr));

	readMess();

	sendto(s, message, messLen, 0, (sockaddr*)&ReceiverAddr, sizeof(ReceiverAddr));
}

void createNamedPipeClient()
{
	HANDLE PipeHandle;
	DWORD BytesWritten;
	if (WaitNamedPipeA(PIPE_NAME, NMPWAIT_WAIT_FOREVER) == 0)
	{
		printf_s("WaitNamedPipe failed with error %d\n", GetLastError());
		return;
	}
	if ((PipeHandle = CreateFileA(PIPE_NAME, GENERIC_READ | GENERIC_WRITE, 0, (LPSECURITY_ATTRIBUTES)NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL)) == INVALID_HANDLE_VALUE)
	{
		printf_s("CreateFile failed with error %d\n", GetLastError());
		return;
	}

	readMess();

	if (WriteFile(PipeHandle, message, messLen, &BytesWritten, NULL)
		== 0)
	{
		printf_s("WriteFile failed with error %d\n", GetLastError());
		CloseHandle(PipeHandle);
		return;
	}
	CloseHandle(PipeHandle);
}

void createWinSockServer()
{
	WSADATA wsd;
	SOCKET s;
	int RetCod;
	sockaddr_in SenderAddr, ReceiverAddr;
	WSAStartup(0x0202, &wsd);
	s = socket(AF_INET, SOCK_DGRAM, 0);
	SenderAddr.sin_addr.S_un.S_addr = inet_addr(IPAddr);
	SenderAddr.sin_port = htons(PORT_AB);
	SenderAddr.sin_family = AF_INET;
	ReceiverAddr.sin_addr.S_un.S_addr = inet_addr(IPAddr);
	ReceiverAddr.sin_port = htons(PORT_AB);
	ReceiverAddr.sin_family = AF_INET;
	bind(s, (sockaddr*)&ReceiverAddr, sizeof(ReceiverAddr));
	int AddrLen = sizeof(SenderAddr);

	char Buffer[BUFFLEN];
	RetCod = recvfrom(s, Buffer, BUFFLEN, 0, (sockaddr*)&SenderAddr, &AddrLen);
	Buffer[RetCod] = 0;
	writeMess(Buffer);
}

void createNamedPipeServer()
{
	HANDLE PipeHandle;
	DWORD BytesRead;
	CHAR buffer[256];
	if ((PipeHandle = CreateNamedPipeA("\\\\.\\Pipe\\PipeB", PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE,
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