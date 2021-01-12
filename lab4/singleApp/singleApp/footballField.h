#pragma once

#include <iostream>
#include <windows.h>
#include "lib/Constants.h"
#include "lib/IntegerChannel.h"
#include "lib/BinarySemaphore.h"

using namespace std;

void terminateField(){
    wcout << "Football field end game!\n";
    ExitThread(0);
}

DWORD WINAPI FootballFieldThreadProc(PVOID arg) {
    cout << "Football field start work!\n";
    //S2
    BinarySemaphore getBallSemaphore(GET_BALL_SEMAPHORE,0,terminateField);
    //C1
    IntegerChannel getBallChannel(GET_BALL_CHANNEL, terminateField);
    //C2
    IntegerChannel returnBallChannel(RETURN_BALL_CHANNEL, terminateField);
    //C3
    IntegerChannel isGoalChannel(FORWARD_FIELD_CHANNEL, terminateField);
    //R2
    IntegerChannel fieldGoalkeeperRendCh1(FIELD_GOALKEEPER_RENDEZVOUS_CH1, terminateField);
    IntegerChannel fieldGoalkeeperRendCh2(FIELD_GOALKEEPER_RENDEZVOUS_CH2, terminateField);
    while (true) {
        getBallChannel.setData(new Message(Owner::FIELD, BALL_MSG));
        cout << "Field set ball to forward for start forwarding\n";
        auto returnBall = returnBallChannel.getData();
        switch (returnBall->sender) {
            // Удар по воротам от нападающего
            case Owner::FORWARD:
                cout << "field getting ball from forward and will send them to goalkeeper\n";
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
                else
                    isGoalChannel.setData(new Message(Owner::FIELD, IS_NOT_GOAL_MSG));
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
        Sleep(PAUSE_BETWEEN_GAMES);
    }
}