#ifndef SINGLE_APP_GOALKEEPER_H
#define SINGLE_APP_GOALKEEPER_H

#include <iostream>
#include <windows.h>
#include "lib/Constants.h"
#include "lib/IntegerChannel.h"
#include "lib/BinarySemaphore.h"
#include "lib/ArrayIntegerChannel.h"

using namespace std;

DWORD WINAPI GoalkeeperThreadProc(PVOID arg) {
    cout << "Goalkeeper start game" << endl;
    int power = MAX_POWER;
    const int skill = Random::nextInt(MAX_SKILL);

    cout << "goalkeeper skill = " << skill << endl;
    IntegerSemaphore endSemaphore(END_GAME_SEMAPHORE);
    //R2
    IntegerChannel fieldGoalkeeperRendCh1(FIELD_GOALKEEPER_RENDEZVOUS_CH1);
    IntegerChannel fieldGoalkeeperRendCh2(FIELD_GOALKEEPER_RENDEZVOUS_CH2);
    //C4
    IntegerChannel doctorRequestChannel(DOCTOR_REQUEST_CHANNEL);
    //C5
    ArrayIntegerChannel doctorResponseChannel(DOCTOR_RESPONSE_CHANNEL);
    int i = 0;
    while (true) {
        auto kickInfo = fieldGoalkeeperRendCh1.getData();
        int goalkeeperChance = countChance(power, skill);
        // результат удара
        int kickResult = goalkeeperChance < kickInfo->data ? IS_GOAL_MSG : IS_NOT_GOAL_MSG;
        wcout << "Goalkeeper " <<  (kickResult == IS_GOAL_MSG ? " playing bad: GOAL!" : " playing good: NOT GOAL!") << endl;
        Message *resultKickMessage = new Message(Owner::GOALKEEPER, kickResult);
        fieldGoalkeeperRendCh2.setData(resultKickMessage);

        // Немного устал
        power -= Random::nextInt(MAX_POWER_DIFF);
        if (power <= 0) {
            // говорим врачу, что требуется лечение
            doctorRequestChannel.setData(new Message(Owner::GOALKEEPER, power));
            // ждём результата лечения
            Message *result = doctorResponseChannel.getData(Owner::GOALKEEPER);
            power += result->data;
            cout << "Goalkeeper relaxed. Power = " << power << "\n";
            delete result;
        }
        delete kickInfo;
        // конец игры
        if (endSemaphore.close(PAUSE_BETWEEN_GAMES)) {
            break;
        }
    }
    ExitThread(0);
}

#endif //SINGLE_APP_GOALKEEPER_H
