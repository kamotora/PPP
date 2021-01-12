#include <iostream>
#include "../lib/ArrayIntegerChannel.h"
#include "../lib/IntegerChannel.h"
#include "../lib/myrandom.h"


void terminateForward(){
    wcout << "Forward end game!\n";
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
    int power = MAX_POWER;
    const int skill = 80;
    cout << "forward skill = " << skill << endl;
    cout << "Forward start game!\n";
    //S1
    BinarySemaphore commandCoachSemaphore(COMMAND_COACH_SEMAPHORE, 0 , terminateForward);
    //S2
    BinarySemaphore getBallSemaphore(GET_BALL_SEMAPHORE, 0, terminateForward);
    //C1
    IntegerChannel getBallChannel(GET_BALL_CHANNEL,terminateForward);
    //R1
    IntegerChannel forwardDefenderRendCh1(FORWARD_DEFENDER_RENDEZVOUS_CH1,terminateForward);
    IntegerChannel forwardDefenderRendCh2(FORWARD_DEFENDER_RENDEZVOUS_CH2,terminateForward);
    //C2
    IntegerChannel returnBallChannel(RETURN_BALL_CHANNEL,terminateForward);
    //C3
    IntegerChannel isGoalChannel(FORWARD_FIELD_CHANNEL,terminateForward);
    //C4
    IntegerChannel doctorRequestChannel(DOCTOR_REQUEST_CHANNEL,terminateForward);
    //C5
    ArrayIntegerChannel doctorResponseChannel(DOCTOR_RESPONSE_CHANNEL,terminateForward);
    //C6
    IntegerChannel coachChannel(COACH_CHANNEL,terminateForward);
    //C7
    IntegerChannel answerToForwardChannel(ANSWER_FORWARD_CHANNEL,terminateForward);

    int goalInfo = 0;
    while (true) {
        // Ждём команды тренера на начало игры
        commandCoachSemaphore.close();
        Message *ball = getBallChannel.getData();
        cout << "\nForward get ball and start forwarding\n";

        // Шанс пройти защитника зависит от усталости и мастерства
        int forwardChance = countChance(power, skill);
        auto defenderChance = forwardDefenderRendCh1.getData();
        int chanceSaveBall = forwardChance > defenderChance->data ? FORWARD_SAVE_BALL : FORWARD_NOT_SAVE_BALL;
//        cout << "forward get data from denender: " << defenderChance->toString() << "\n";
        Message *resultMessage = new Message(Owner::FORWARD, chanceSaveBall);
        forwardDefenderRendCh2.setData(resultMessage);
        if (chanceSaveBall == FORWARD_SAVE_BALL) {
            cout << "Forward save ball and continue forwarding\n";
            // Наносим удар по воротам
            returnBallChannel.setData(new Message(Owner::FORWARD, countChance(power, skill)));
            auto isGoalMsg = isGoalChannel.getData();
            switch (isGoalMsg->data) {
                case IS_GOAL_MSG:
                    cout << "Forward make goal!\n";
                    break;
                case IS_NOT_GOAL_MSG:
                    cout << "Forward not make goal\n";
                    break;
                default:
                    cerr << "Unknown is goal msg from field to forward\n";
            }
            goalInfo = isGoalMsg->data;
            // Ударил по воротам, ещё больше устал
            power -= Random::nextInt(MAX_POWER_DIFF);
            delete isGoalMsg;
        } else if (chanceSaveBall == FORWARD_NOT_SAVE_BALL) {
            cout << "Forward send ball to defender \n";
            goalInfo = IS_NOT_GOAL_MSG;
        } else
            cout << "Error in rendezvous for forward . Value " << chanceSaveBall << "\n";
        // Устал после обвода защитника
        power -= Random::nextInt(MAX_POWER_DIFF);
        if (power <= 0) {
            // говорим врачу, что требуется лечение
            doctorRequestChannel.setData(new Message(Owner::FORWARD, power));
            cout << "Forward tired (power = " << power << ") and send request to doctor\n";
            // ждём результата лечения
            Message *result = doctorResponseChannel.getData(Owner::FORWARD);
            power += result->data;
            cout << "Forward relaxed. Power = " << power << "\n";
            delete result;
        }

        // Сообщаем полученный результат
        coachChannel.setData(new Message(Owner::FORWARD, goalInfo));
        cout << "Forward told coach about game result \n";
        // Получаем совет от тренера
        auto coachAnswer = answerToForwardChannel.getData();
        if (coachAnswer->data == GOOD_FORWARD_MSG)
            cout << "Coach say to forward: " << "You make goal! Well done!!!\n";
        else
            cout << "Coach say to forward: " << "You not make goal! Bad, try once more!!!\n";
        // Удаляем, т.к. прочитали и уже не пригодится
        delete defenderChance;
        delete ball;
        Sleep(PAUSE_BETWEEN_GAMES);
    }
}

