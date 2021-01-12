#include <iostream>
#include <windows.h>
#include "lib/myrandom.h"
#include "coach.h"
#include "defender.h"
#include "doctor.h"
#include "footballField.h"
#include "goalkeeper.h"
#include "forward.h"

using namespace std;

HANDLE createThread(LPTHREAD_START_ROUTINE func) {
    HANDLE hThread = CreateThread(NULL, 0, func, NULL, 0, NULL);
    if (hThread == NULL)
        cerr << GetLastError() << endl;
    return hThread;
}

int main() {
    cout << "Start program!\n";

    HANDLE forwardThread = createThread(ForwardThreadProc);
    Sleep(100);
    HANDLE footballFieldThread = createThread(FootballFieldThreadProc);
    Sleep(100);
    HANDLE goalkeeperThread = createThread(GoalkeeperThreadProc);
    Sleep(100);
    HANDLE coachThread = createThread(CoachThreadProc);
    Sleep(100);
    HANDLE defenderThread = createThread(DefenderThreadProc);
    Sleep(100);
    HANDLE doctorThread = createThread(DoctorThreadProc);

    WaitForSingleObject(forwardThread, INFINITE);
    WaitForSingleObject(doctorThread, INFINITE);
    WaitForSingleObject(coachThread, INFINITE);
    WaitForSingleObject(footballFieldThread, INFINITE);
    WaitForSingleObject(defenderThread, INFINITE);
    WaitForSingleObject(goalkeeperThread, INFINITE);

    cout << "End program!\n";
    return 0;
}
