#pragma once

#include "lib/BinarySemaphore.h"
#include "lib/IntegerSemaphore.h"
#include <iostream>
#include "helper.h"
#include "Message.h"
#include "Signal.h"

using namespace std;

class IntegerChannel {
private:
    BinarySemaphore *freeSemaphore, *emptySemaphore;
    HANDLE fileMem;
    wstring name;
    void *buffer;
    Signal *endGameSignal;
    functionType terminateFunc;
public:
    IntegerChannel(const wstring &name, functionType terminateFunc = nullptr);

    void setData(Message *data, int timeout = MAX_WAIT_TIME);

    Message *getData(int timeout = MAX_WAIT_TIME);

    ~IntegerChannel();
};
