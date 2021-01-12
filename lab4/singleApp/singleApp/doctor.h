#pragma once

#include <iostream>
#include <windows.h>
#include "lib/Constants.h"
#include "lib/IntegerChannel.h"
#include "lib/BinarySemaphore.h"

using namespace std;

void terminateDoctor(){
    wcout << "Doctor end game!\n";
    ExitThread(0);
}

DWORD WINAPI DoctorThreadProc(PVOID arg) {
    cout << "Doctor start work" << endl;
    //C4
    IntegerChannel doctorRequestChannel(DOCTOR_REQUEST_CHANNEL, terminateDoctor);
    //C5
    ArrayIntegerChannel doctorResponseChannel(DOCTOR_RESPONSE_CHANNEL, terminateDoctor);
    //C6
    IntegerChannel coachChannel(COACH_CHANNEL, terminateDoctor);
    int i = 0;
    while (true) {
        // Получаем (ждём) запрос на лечение
        auto request = doctorRequestChannel.getData();
        //Уставший игрок
        Owner tiredPlayer = request->sender;
        // Говорим тренеру, что игрок устал
        switch (tiredPlayer) {
            case Owner::FORWARD:
                coachChannel.setData(new Message(Owner::DOCTOR, FORWARD_TIRED_MSG));
                cout << "Doctor told coach that forward was tired\n";
                break;
            case Owner::DEFENDER:
                coachChannel.setData(new Message(Owner::DOCTOR, DEFENDER_TIRED_MSG));
                cout << "Doctor told coach that defender was tired\n";
                break;
            case Owner::GOALKEEPER:
                coachChannel.setData(new Message(Owner::DOCTOR, GOALKEEPER_TIRED_MSG));
                cout << "Doctor told coach that defender was tired\n";
                break;
            default:
                cerr << "Unknown tired player type\n";
        }

        // Лечим игрока с разным успехом
        // здесь - указываем в качестве отправителя - получателя
        auto response = new Message(tiredPlayer, Random::nextInt(MAX_POWER / 2, MAX_POWER));
        doctorResponseChannel.setData(response);

        //Говорим тренеру, что игрок готов
        switch (tiredPlayer) {
            case Owner::FORWARD:
                coachChannel.setData(new Message(Owner::DOCTOR, FORWARD_RELAX_MSG));
                break;
            case Owner::DEFENDER:
                coachChannel.setData(new Message(Owner::DOCTOR, DEFENDER_RELAX_MSG));
                break;
            case Owner::GOALKEEPER:
                coachChannel.setData(new Message(Owner::DOCTOR, GOALKEEPER_RELAX_MSG));
                break;
            default:
                cerr << "Unknown relax player type\n";
        }
        delete request;
        Sleep(PAUSE_BETWEEN_GAMES);
    }
}
