#include <iostream>
#include "../lib/ArrayIntegerChannel.h"
#include "../lib/IntegerChannel.h"

void terminateCoach(){
    wcout << "Coach end game!\n";
    system("pause");
    ExitThread(0);
}

int main() {
    cout << "Coach start game!\n";
    //S1
    BinarySemaphore commandCoachSemaphore(COMMAND_COACH_SEMAPHORE);
    //C6
    IntegerChannel coachChannel(COACH_CHANNEL, terminateCoach);
    //C7
    IntegerChannel answerToForwardChannel(ANSWER_FORWARD_CHANNEL, terminateCoach);
    Signal endGameSignal;
    bool isDefenderTired = false, isGoalKeeperTired = false, isForwardTired = false;
    int i = 0;
    while (true) {
        // Если все игроки готовы, начинаем игру
        if (!isDefenderTired && !isGoalKeeperTired && !isForwardTired)
            commandCoachSemaphore.open();
        else
            cout << "We have tired players, coach not start game\n";
        auto message = coachChannel.getData();

//        wcout << "coach get message " << message->toWstring();
        switch (message->sender) {
            case Owner::DOCTOR:
                switch (message->data) {
                    case FORWARD_TIRED_MSG:
                        isForwardTired = true;
                        cout << "Coach mark forward as tired\n";
                        break;
                    case DEFENDER_TIRED_MSG:
                        isDefenderTired = true;
                        cout << "Coach mark defender as tired\n";
                        break;
                    case GOALKEEPER_TIRED_MSG:
                        isGoalKeeperTired = true;
                        cout << "Coach mark goalkeeper as tired\n";
                        break;
                    case FORWARD_RELAX_MSG:
                        isForwardTired = false;
                        cout << "Coach mark forward as relaxed\n";
                        break;
                    case DEFENDER_RELAX_MSG:
                        isDefenderTired = false;
                        cout << "Coach mark defender as relaxed\n";
                        break;
                    case GOALKEEPER_RELAX_MSG:
                        isGoalKeeperTired = false;
                        cout << "Coach mark goalkeeper as relaxed\n";
                        break;
                    default:
                        cerr << "Unknown doctor for coach message\n";
                }
                break;
            case Owner::FORWARD:
                wcout << "============ Game " << i << " ended ===========\n";
                i++;
                if (message->data == IS_GOAL_MSG)
                    answerToForwardChannel.setData(new Message(Owner::COACH, GOOD_FORWARD_MSG));
                else if (message->data == IS_NOT_GOAL_MSG)
                    answerToForwardChannel.setData(new Message(Owner::COACH, BAD_FORWARD_MSG));
                else
                    cerr << "Unknown forward message for coach";
                break;
            default:
                cerr << "Unknown message sender for coach \n";
        }

        // Удаляем, т.к. прочитали и уже не пригодится
        delete message;
        // Сыграли достаточно кол-во раз, завершаем игру
        if(i == COUNT_GAMES){
            endGameSignal.setSignal();
            cout << "=== END GAME!!! Coach end game and say other players go to home! ===\n";
            ExitThread(0);
        }
        Sleep(PAUSE_BETWEEN_GAMES);
    }
}
