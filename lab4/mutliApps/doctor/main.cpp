#include <iostream>
#include "../lib/ArrayIntegerChannel.h"
#include "../lib/myrandom.h"
#include "../lib/IntegerChannel.h"

void terminateDoctor(){
    wcout << "Doctor end game!\n";
    system("pause");
    ExitThread(0);
}
BOOL ExitHandler(DWORD fdwCtrlType) {
    Signal endGame;
    endGame.setSignal();
    return TRUE;
}

int main() {
    SetConsoleCtrlHandler(
            (PHANDLER_ROUTINE) ExitHandler,  // функция обработчика
            TRUE);
    cout << "Doctor start work" << endl;
    //C4
    IntegerChannel doctorRequestChannel(DOCTOR_REQUEST_CHANNEL, terminateDoctor);
    //C5
    ArrayIntegerChannel doctorResponseChannel(DOCTOR_RESPONSE_CHANNEL, terminateDoctor);
    //C6
    IntegerChannel coachChannel(COACH_CHANNEL, terminateDoctor);
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
