#ifndef SINGLEAPP_SIGNAL_H
#define SINGLEAPP_SIGNAL_H

#include <iostream>
#include "helper.h"
#include "Message.h"
#include "Constants.h"

using namespace std;

/**
 * Сигнал наоборот
 * handle сразу в сигнальном состоянии
 * setSignal переводит в несигнальное
 * Если waitForSingleObject не дожидается сигнального состояния, isSignal - true
 * Иначе false
 */
class Signal {
private:
    HANDLE handle;
    wstring name;
public:
    explicit Signal(const wstring &name = END_GAME_SEMAPHORE);
    ~Signal();
    bool isSignal(int timeout = SIGNAL_WAIT_TIME);
    void setSignal();
};


#endif //SINGLEAPP_SIGNAL_H
