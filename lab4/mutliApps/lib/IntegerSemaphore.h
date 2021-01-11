#pragma once

#include <windows.h>
#include "helper.h"

class IntegerSemaphore {
private:
    HANDLE sem;
public:
    explicit IntegerSemaphore(const wstring &name, int startState = 0, int maxState = 7);

    ~IntegerSemaphore();

    bool open(int n);

    bool close(DWORD milliseconds = INFINITE);
};
