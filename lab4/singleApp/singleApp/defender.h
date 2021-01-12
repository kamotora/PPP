#pragma once

#include <iostream>
#include <windows.h>
#include "lib/Constants.h"
#include "lib/IntegerChannel.h"
#include "lib/BinarySemaphore.h"
#include "lib/ArrayIntegerChannel.h"

using namespace std;

void terminateDefender(){
    wcout << "Defender end game!\n";
    ExitThread(0);
}

DWORD WINAPI DefenderThreadProc(PVOID arg) {
    int power = MAX_POWER;
    const int skill = Random::nextInt(MAX_SKILL);

    cout << "Defender start game \n" << endl;

    cout << "defender skill = " << skill << endl;
    //R1
    IntegerChannel forwardDefenderRendCh1(FORWARD_DEFENDER_RENDEZVOUS_CH1, terminateDefender);
    IntegerChannel forwardDefenderRendCh2(FORWARD_DEFENDER_RENDEZVOUS_CH2, terminateDefender);
    //C2
    IntegerChannel returnBallChannel(RETURN_BALL_CHANNEL, terminateDefender);
    //C4
    IntegerChannel doctorRequestChannel(DOCTOR_REQUEST_CHANNEL, terminateDefender);
    //C5
    ArrayIntegerChannel doctorResponseChannel(DOCTOR_RESPONSE_CHANNEL, terminateDefender);
    while (true) {
        // Передаём информацию об усталости и навыках защитника
        int defenderChance = countChance(power, skill);
        Message *initMessage = new Message(Owner::DEFENDER, defenderChance, State::INIT_DATA);
        forwardDefenderRendCh1.setData(initMessage);
        cout << "Defender try to get ball from forward\n";
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
        delete getResult;
        Sleep(PAUSE_BETWEEN_GAMES);
    }
}
