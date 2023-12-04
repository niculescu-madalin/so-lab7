#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define INTSIZE sizeof(int)

int isPrime(int number) {
    if (number < 2) {
        return 0;
    }

    for (int i = 2; i * i <= number; ++i) {
        if (number % i == 0) {
            return 0;
        }
    }

    return 1;
}

void findPrimes(int child_num, int start, int stop, HANDLE hPipe) {
    for (int i = start; i <= stop; ++i) {
        if (isPrime(i)) {
            WriteFile(hPipe, &i, INTSIZE, NULL, NULL);
        }
    }
}

int main() {
    HANDLE hPipe = NULL;
    DWORD bytesRead;
    int received_data;
    const int NUM_P = 10;
    int i;

 
    SECURITY_ATTRIBUTES sa = { 1 };
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = TRUE;

    if (!CreatePipe(&hPipe, NULL, &sa, 0)) {
        fprintf(stderr, "Error creating pipe\n");
        return 1;
    }


    for (i = 0; i < NUM_P; ++i) {
        STARTUPINFO si;
        PROCESS_INFORMATION pi;

        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        WCHAR command[50];
        swprintf_s(command, L"child.exe %d %d %d", i + 1, 1000 * i + 1, 1000 * (i + 1));

        if (!CreateProcessW(NULL, command, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
            fprintf(stderr, "Error creating process\n");
            return 1;
        }

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    CloseHandle(hPipe);

    HANDLE hNewPipe = CreateNamedPipe(L"\\\\.\\pipe\\myPipe", PIPE_ACCESS_INBOUND, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT, 1, 0, 0, 0, NULL);

    ConnectNamedPipe(hNewPipe, NULL);

    while (ReadFile(hNewPipe, &received_data, INTSIZE, &bytesRead, NULL)) {
        if (bytesRead > 0) {
            printf("%d\n", received_data);
        }
        else {
            // No data was read, break out of the loop
            break;
        }
    }

    CloseHandle(hNewPipe); // Close the new handle

    return 0;
}
