#include "winsock2.h"
#include <windows.h>
#include <stdio.h>
#include <fstream>

#pragma warning(disable: 4996)
#define PORT_AB 4444
#define PORT_BA 5555
#define PORT_AE 6666
#define BUFFLEN 4096

char info[] = "Viberit rezhim roboti:\n1 - Vidpraviti povidomlennja na vuzol B\n2 - Vidpraviti povidomlennja na vuzol E\n3 - Otrimati povidomlennja vid vuzla B\n-----\n4 - Zavershiti robotu vuzla A\n";
char IPAddr[] = "127.0.0.1";

void readMess();
void writeMess(char message[]);

void createWinSockClientB();
void createWinSockClientE();
void createWinSockServer();

char* message = new char[4096];
int messLen = 0;

int main()
{
    printf(info);

    int command;

    scanf("%d", &command);

    while (command >= 1 && command <= 3)
    {
        switch (command)
        {
        case 1:
            createWinSockClientB();
            break;
        case 2:
            createWinSockClientE();
            break;
        case 3:
            createWinSockServer();
            break;
        default:
            break;
        }

        printf(info);
        scanf("%d", &command);
    }

    printf("\n\nVuzol A zavershiv robotu...\n");

    return 0;
}

void readMess()
{
    char mess[4096];

    printf("Vvedit povidomlennia ->\n");
    scanf("%4095s", mess);

    message = strcpy(message, mess);
    messLen = strlen(message);
}

void writeMess(char message[])
{
    printf("--- --- MESSAGE --- ---\n");
    printf(message);
    printf("\n--- --- --- --- --- ---\n");

    std::ofstream out;
    out.open("output.txt");
    if (out.is_open())
    {
        out << message;
        out << "\n";
    }
}

void createWinSockClientB()
{
    WSADATA wsd;
    SOCKET s;
    sockaddr_in SenderAddr, ReceiverAddr;
    WSAStartup(0x0202, &wsd);
    s = socket(AF_INET, SOCK_DGRAM, 0);

    SenderAddr.sin_addr.S_un.S_addr = inet_addr(IPAddr);
    SenderAddr.sin_port = htons(PORT_AB);
    SenderAddr.sin_family = AF_INET;
    ReceiverAddr.sin_addr.S_un.S_addr = inet_addr(IPAddr);
    ReceiverAddr.sin_port = htons(PORT_AB);
    ReceiverAddr.sin_family = AF_INET;

    bind(s, (sockaddr*)&SenderAddr, sizeof(SenderAddr));

    readMess();

    sendto(s, message, messLen, 0, (sockaddr*)&ReceiverAddr, sizeof(ReceiverAddr));
}

void createWinSockClientE()
{
    WSADATA wsd;
    SOCKET s;
    sockaddr_in SenderAddr, ReceiverAddr;
    WSAStartup(0x0202, &wsd);
    s = socket(AF_INET, SOCK_DGRAM, 0);

    SenderAddr.sin_addr.S_un.S_addr = inet_addr(IPAddr);
    SenderAddr.sin_port = htons(PORT_AE);
    SenderAddr.sin_family = AF_INET;
    ReceiverAddr.sin_addr.S_un.S_addr = inet_addr(IPAddr);
    ReceiverAddr.sin_port = htons(PORT_AE);
    ReceiverAddr.sin_family = AF_INET;

    bind(s, (sockaddr*)&SenderAddr, sizeof(SenderAddr));

    readMess();

    sendto(s, message, messLen, 0, (sockaddr*)&ReceiverAddr, sizeof(ReceiverAddr));
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
    SenderAddr.sin_port = htons(PORT_BA);
    SenderAddr.sin_family = AF_INET;
    ReceiverAddr.sin_addr.S_un.S_addr = inet_addr(IPAddr);
    ReceiverAddr.sin_port = htons(PORT_BA);
    ReceiverAddr.sin_family = AF_INET;
    bind(s, (sockaddr*)&ReceiverAddr, sizeof(ReceiverAddr));
    int AddrLen = sizeof(SenderAddr);

    char Buffer[BUFFLEN];
    RetCod = recvfrom(s, Buffer, BUFFLEN, 0, (sockaddr*)&SenderAddr, &AddrLen);
    Buffer[RetCod] = 0;
    writeMess(Buffer);
}