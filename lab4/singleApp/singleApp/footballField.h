#pragma once

#include <iostream>
#include <windows.h>
#include "lib/Constants.h"
#include "lib/IntegerChannel.h"
#include "lib/BinarySemaphore.h"

using namespace std;


DWORD WINAPI FootballFieldThreadProc(PVOID arg) {
    cout << "Football field start work!\n";
    IntegerSemaphore endSemaphore(END_GAME_SEMAPHORE);
    //S2
    BinarySemaphore getBallSemaphore(GET_BALL_SEMAPHORE);
    //C1
    IntegerChannel getBallChannel(GET_BALL_CHANNEL);
    //C2
    IntegerChannel returnBallChannel(RETURN_BALL_CHANNEL);
    //C3
    IntegerChannel isGoalChannel(FORWARD_FIELD_CHANNEL);
    //R2
    IntegerChannel fieldGoalkeeperRendCh1(FIELD_GOALKEEPER_RENDEZVOUS_CH1);
    IntegerChannel fieldGoalkeeperRendCh2(FIELD_GOALKEEPER_RENDEZVOUS_CH2);
    int i = 0;
    while (true) {
        //getBallSemaphore.close();
        // 10 == мяч)
        getBallChannel.setData(new Message(Owner::FIELD, BALL_MSG));
        cout << "Field set ball to forward for start forwarding\n";
        auto returnBall = returnBallChannel.getData();
        switch (returnBall->sender) {
            // Удар по воротам от нападающего
            case Owner::FORWARD:
                // Удар хороший, направляем в ворота
                if (returnBall->data > 50) {
                    auto initMsg = new Message(Owner::FIELD, returnBall->data, State::INIT_DATA);
                    fieldGoalkeeperRendCh1.setData(initMsg);
                    cout << "field send ball to goalkeeper\n";
                    Message *resultMsg = fieldGoalkeeperRendCh2.getData();
                    if (resultMsg->data == IS_GOAL_MSG)
                        isGoalChannel.setData(new Message(Owner::FIELD, IS_GOAL_MSG));
                    else if (resultMsg->data == IS_NOT_GOAL_MSG)
                        isGoalChannel.setData(new Message(Owner::FIELD, IS_NOT_GOAL_MSG));
                    else
                        cerr << "Unknown rendezvous result in field = " << resultMsg->toString();
                    delete resultMsg;
                }
                break;

                // Успешно отобранный мяч от защитника
            case Owner::DEFENDER:
                cout << "Field get ball from defender, forwarding ended \n";
                break;
            default:
                cout << "Error message in footballField " << returnBall->toString();
                break;
        }
        delete returnBall;
        // конец игры
        if (endSemaphore.close(PAUSE_BETWEEN_GAMES)) {
            break;
        }
    }
    ExitThread(0);
}