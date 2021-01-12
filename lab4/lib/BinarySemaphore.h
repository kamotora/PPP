#pragma once

#include "windows.h"
#include <iostream>
#include "helper.h"
#include "Constants.h"
#include "Signal.h"

using namespace std;

class BinarySemaphore {
private:
    HANDLE sem;
    wstring name;
    Signal *endGameSignal;
    functionType terminateFunc;
public:
    BinarySemaphore(const wstring &name, int startState, functionType termFunc);

    ~BinarySemaphore();

    void open();

    void close(DWORD milliseconds = MAX_WAIT_TIME);
};
