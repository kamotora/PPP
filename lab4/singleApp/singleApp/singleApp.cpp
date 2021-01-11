#include <iostream>
#include <windows.h>
#include "lib/myrandom.h"
#include "coach.h"
#include "defender.h"
#include "doctor.h"
#include "footballField.h"
#include "goalkeeper.h"
#include "forward.h"
#include "lib/Constants.h"
#include "lib/IntegerSemaphore.h"

using namespace std;

void init() {
    IntegerSemaphore endSemaphore(END_GAME_SEMAPHORE);
    //S1
    BinarySemaphore commandCoachSemaphore(COMMAND_COACH_SEMAPHORE);
    //S2
    BinarySemaphore getBallSemaphore(GET_BALL_SEMAPHORE);
    //C1
    IntegerChannel getBallChannel(GET_BALL_CHANNEL);
    //C2
    IntegerChannel returnBallChannel(RETURN_BALL_CHANNEL);
    //C3
    IntegerChannel isGoalChannel(FORWARD_FIELD_CHANNEL);
    //C4
    IntegerChannel doctorRequestChannel(DOCTOR_REQUEST_CHANNEL);
    //C5
    ArrayIntegerChannel doctorResponseChannel(DOCTOR_RESPONSE_CHANNEL);
    //C6
    IntegerChannel coachChannel(COACH_CHANNEL);
    //C7
    IntegerChannel answerToForwardChannel(ANSWER_FORWARD_CHANNEL);
    //R1
    IntegerChannel forwardDefenderRendCh1(FORWARD_DEFENDER_RENDEZVOUS_CH1);
    IntegerChannel forwardDefenderRendCh2(FORWARD_DEFENDER_RENDEZVOUS_CH2);
    //R2
    IntegerChannel fieldGoalkeeperRendCh1(FIELD_GOALKEEPER_RENDEZVOUS_CH1);
    IntegerChannel fieldGoalkeeperRendCh2(FIELD_GOALKEEPER_RENDEZVOUS_CH2);
}


HANDLE createThread(LPTHREAD_START_ROUTINE func) {
    HANDLE hThread = CreateThread(NULL, 0, func, NULL, 0, NULL);
    if (hThread == NULL)
        cerr << GetLastError() << endl;
    return hThread;
}

int main() {
    init();
//    Random::randomSeed();
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
