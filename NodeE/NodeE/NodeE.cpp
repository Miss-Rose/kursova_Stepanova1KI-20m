#include "winsock2.h"
#include <windows.h>
#include <stdio.h>
#include <fstream>

#pragma warning(disable: 4996)
#define PORT_AE 6666
#define BUFFLEN 4096

char info[] = "Viberit rezhim roboti:\n1 - Otrimati povidomlennja vid vuzla A\n2 - Otrimati povidomlennja vid vuzla D\n-----\n3 - Zavershiti robotu vuzla E\n";
char IPAddr[] = "127.0.0.1";

void writeMess(char message[]);

void createWinSockServer();
void createMailSlotServer();

int main()
{
    printf(info);

    int command;

    scanf("%d", &command);

    while (command >= 1 && command <= 2)
    {
        switch (command)
        {
        case 1:
            createWinSockServer();
            break;
        case 2:
            createMailSlotServer();
            break;
        default:
            break;
        }

        printf(info);
        scanf("%d", &command);
    }

    printf("\n\nVuzol E zavershiv robotu...\n");

    return 0;
}

void writeMess(char message[])
{
    printf("--- --- MESSAGE --- ---\n");
    printf(message);
    printf("\n--- --- --- --- --- ---\n");
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
    SenderAddr.sin_port = htons(PORT_AE);
    SenderAddr.sin_family = AF_INET;
    ReceiverAddr.sin_addr.S_un.S_addr = inet_addr(IPAddr);
    ReceiverAddr.sin_port = htons(PORT_AE);
    ReceiverAddr.sin_family = AF_INET;
    bind(s, (sockaddr*)&ReceiverAddr, sizeof(ReceiverAddr));
    int AddrLen = sizeof(SenderAddr);

    char Buffer[BUFFLEN];
    RetCod = recvfrom(s, Buffer, BUFFLEN, 0, (sockaddr*)&SenderAddr, &AddrLen);
    Buffer[RetCod] = 0;
    writeMess(Buffer);
}

void createMailSlotServer()
{
    HANDLE Mailslot;
    char buffer[256];
    DWORD NumberOfBytesRead;
    if ((Mailslot = CreateMailslotA("\\\\.\\Mailslot\\MailSlotE", 0,
        MAILSLOT_WAIT_FOREVER, NULL)) == INVALID_HANDLE_VALUE)
    {
        printf("Failed to create a mailslot %d\n", GetLastError());
        return;
    }
    ReadFile(Mailslot, buffer, 256, &NumberOfBytesRead, NULL);
    buffer[NumberOfBytesRead] = 0;

    writeMess(buffer);

    CloseHandle(Mailslot);
}