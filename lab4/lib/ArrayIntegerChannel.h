#pragma once

#include "BinarySemaphore.h"
#include <iostream>
#include "helper.h"
#include "Message.h"
#include "BinarySemaphore.h"
#include "vector"
#include "Signal.h"

using namespace std;

class ArrayIntegerChannel {
private:
    BinarySemaphore *emptySemaphore;
    HANDLE fileMem;
    wstring name;
    void *buffer;
    BOOL writeMessages(vector<Message *> messages);
    vector<struct Message *> getMessages();
    Signal *endGameSignal;
    functionType terminateFunc;
public:
    ArrayIntegerChannel(const wstring &name, functionType terminateFunc = nullptr);

    void setData(Message *data, int timeout = MAX_WAIT_TIME);

    Message *getData(Owner receiver, int timeout = MAX_WAIT_TIME);

    Message* findMessageInBuffer(Owner receiver);

    bool addMessage(Message* data);

    ~ArrayIntegerChannel();
};
