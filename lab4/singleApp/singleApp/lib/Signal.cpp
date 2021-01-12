//
// Created by honor on 12.01.2021.
//

#include "Signal.h"

Signal::Signal(const wstring &name) {
    this->name = name;
    this->handle = OpenEventW(EVENT_ALL_ACCESS, TRUE, name.c_str());
    if (!this->handle)
        this->handle = CreateEventW(NULL, TRUE, TRUE, name.c_str());

}

bool Signal::isSignal(int timeout) {
    DWORD result = WaitForSingleObject(handle, timeout);
    return result == WAIT_TIMEOUT;
}

void Signal::setSignal() {
    ResetEvent(handle);
}

Signal::~Signal() {
    CloseHandle(handle);
}
