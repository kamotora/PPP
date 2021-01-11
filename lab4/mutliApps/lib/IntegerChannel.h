#pragma once

#include "../lib/BinarySemaphore.h"
#include <iostream>
#include "helper.h"
#include "Message.h"

using namespace std;

class IntegerChannel {
private:
    BinarySemaphore *freeSemaphore, *emptySemaphore;
    HANDLE fileMem;
    wstring name;
    void *buffer;
public:
    explicit IntegerChannel(const wstring &name);

    void setData(Message *data, int timeout = MAX_WAIT_TIME);

    Message *getData(int timeout = MAX_WAIT_TIME);

    ~IntegerChannel();
};
