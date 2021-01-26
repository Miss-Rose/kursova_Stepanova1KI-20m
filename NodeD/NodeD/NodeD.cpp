#include "winsock2.h"
#include <windows.h>
#include <stdio.h>
#include <fstream>

#pragma warning(disable: 4996)
#define PIPE_NAME "\\\\.\\Pipe\\PipeB"

char info[] = "Viberit rezhim roboti:\n1 - Vidpraviti povidomlennja na vuzol B\n2 - Vidpraviti povidomlennja na vuzol E\n-----\n3 - Zavershiti robotu vuzla D\n";

void readMess();

void createNamedPipeClient();
void createMailSlotClient();

char* message = new char[4096];
int messLen = 0;

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
            createNamedPipeClient();
            break;
        case 2:
            createMailSlotClient();
            break;
        default:
            break;
        }

        printf(info);
        scanf("%d", &command);
    }

    printf("\n\nVuzol D zavershiv robotu...\n");

    return 0;
}

void readMess()
{
    char mess[2048];
    char mess2[2048];

    std::ifstream inf("input1.txt");
    inf >> mess;

    std::ifstream inf2("input2.txt");
    inf2 >> mess2;

    strcat(mess, mess2);

    message = strcpy(message, mess);
    messLen = strlen(message);
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

void createMailSlotClient()
{
    HANDLE Mailslot;
    DWORD BytesWritten;
    char ServerName[256] = "\\\\.\\Mailslot\\MailSlotE";
    if ((Mailslot = CreateFileA(ServerName, GENERIC_WRITE,
        FILE_SHARE_READ, NULL, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL)) == INVALID_HANDLE_VALUE)
    {
        printf("CreateFile failed with error %d\n", GetLastError());
        return;
    }

    readMess();

    if (WriteFile(Mailslot, message, messLen, &BytesWritten,
        NULL) == 0)
    {
        printf("WriteFile failed with error %d\n", GetLastError());
        return;
    }
    CloseHandle(Mailslot);
}