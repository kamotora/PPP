#include <iostream>
#include "../../lib/ArrayIntegerChannel.h"
#include "../../lib/IntegerChannel.h"
#include "../../lib/myrandom.h"

void terminateGoalkeeper() {
    wcout << "Goalkeeper end game!\n";
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
    cout << "Goalkeeper start game" << endl;
    int power = MAX_POWER;
    const int skill = Random::nextInt(MAX_SKILL);

    cout << "goalkeeper skill = " << skill << endl;
    //R2
    IntegerChannel fieldGoalkeeperRendCh1(FIELD_GOALKEEPER_RENDEZVOUS_CH1, terminateGoalkeeper);
    IntegerChannel fieldGoalkeeperRendCh2(FIELD_GOALKEEPER_RENDEZVOUS_CH2, terminateGoalkeeper);
    //C4
    IntegerChannel doctorRequestChannel(DOCTOR_REQUEST_CHANNEL, terminateGoalkeeper);
    //C5
    ArrayIntegerChannel doctorResponseChannel(DOCTOR_RESPONSE_CHANNEL, terminateGoalkeeper);
    while (true) {
        auto kickInfo = fieldGoalkeeperRendCh1.getData();
        int goalkeeperChance = countChance(power, skill);
        // результат удара
        int kickResult = goalkeeperChance < kickInfo->data ? IS_GOAL_MSG : IS_NOT_GOAL_MSG;
        wcout << "Goalkeeper " << (kickResult == IS_GOAL_MSG ? " playing bad: GOAL!" : " playing good: NOT GOAL!")
              << endl;
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
        Sleep(PAUSE_BETWEEN_GAMES);
    }
}
