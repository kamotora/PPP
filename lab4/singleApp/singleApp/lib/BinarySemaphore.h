#pragma once

#include "windows.h"
#include <iostream>
#include "helper.h"
#include "Constants.h"

using namespace std;

class BinarySemaphore {
private:
    HANDLE sem;
    wstring name;
public:
    BinarySemaphore(const wstring &name, int startState = 0);

    ~BinarySemaphore();

    bool open();

    bool close(DWORD milliseconds = MAX_WAIT_TIME);
};
