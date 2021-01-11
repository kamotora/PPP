#pragma once

#include <iostream>
#include <windows.h>
#include "lib/Constants.h"
#include "lib/IntegerChannel.h"
#include "lib/BinarySemaphore.h"
#include "lib/ArrayIntegerChannel.h"

using namespace std;

DWORD WINAPI DefenderThreadProc(PVOID arg) {
    int power = MAX_POWER;
    const int skill = Random::nextInt(MAX_SKILL);

    cout << "Defender start game \n" << endl;

    cout << "defender skill = " << skill << endl;
    IntegerSemaphore endSemaphore(END_GAME_SEMAPHORE);
    //R1
    IntegerChannel forwardDefenderRendCh1(FORWARD_DEFENDER_RENDEZVOUS_CH1);
    IntegerChannel forwardDefenderRendCh2(FORWARD_DEFENDER_RENDEZVOUS_CH2);
    //C2
    IntegerChannel returnBallChannel(RETURN_BALL_CHANNEL);
    //C4
    IntegerChannel doctorRequestChannel(DOCTOR_REQUEST_CHANNEL);
    //C5
    ArrayIntegerChannel doctorResponseChannel(DOCTOR_RESPONSE_CHANNEL);
    int i = 0;
    while (true) {
        // Передаём информацию об усталости и навыках защитника
        int defenderChance = countChance(power, skill);
        Message *initMessage = new Message(Owner::DEFENDER, defenderChance, State::INIT_DATA);
        forwardDefenderRendCh1.setData(initMessage);
        auto getResult = forwardDefenderRendCh2.getData();
        defenderChance = getResult->data;
        if (defenderChance == FORWARD_NOT_SAVE_BALL) // Получили мяч
        {
            cout << "Defender get ball from forward\n";
            // Отдаём мяч полю
            returnBallChannel.setData(new Message(Owner::DEFENDER, BALL_MSG));
        } else if (defenderChance == FORWARD_SAVE_BALL) // Нападающий оставил мяч
            cout << "Defender not get ball \n";
        else
            cout << "Error in rendevous for defender. Value " << getResult->toString() << "\n";

        // Устал после отбора мяча
        power -= Random::nextInt(MAX_POWER_DIFF);
        if (power <= 0) {
            // говорим врачу, что требуется лечение
            doctorRequestChannel.setData(new Message(Owner::DEFENDER, power));
            // ждём результата лечения
            Message *result = doctorResponseChannel.getData(Owner::DEFENDER);
            power += result->data;
            cout << "Defender relaxed. Power = " << power << "\n";
            delete result;
        }
        // конец игры
        if (endSemaphore.close(PAUSE_BETWEEN_GAMES)) {
            break;
        }
        delete getResult;
    }
    ExitThread(0);
}
