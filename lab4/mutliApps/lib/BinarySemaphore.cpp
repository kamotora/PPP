#include "BinarySemaphore.h"

BinarySemaphore::BinarySemaphore(const wstring &name, int startState) {
    this->name = name;
    sem = OpenSemaphoreW(SEMAPHORE_ALL_ACCESS, true, name.c_str());
    int s = (startState > 0) ? 1 : 0;
    if (sem == NULL) {
        sem = CreateSemaphoreW(NULL, s, 1, name.c_str());
    }
}

BinarySemaphore::~BinarySemaphore() {
    CloseHandle(sem);
}


// V()
bool BinarySemaphore::open() {
    BOOL res = ReleaseSemaphore(sem, 1, NULL);
    return res == WAIT_TIMEOUT;
}

// P()
bool BinarySemaphore::close(DWORD milliseconds) {
    DWORD result = WaitForSingleObject(sem, milliseconds);
    return result != WAIT_TIMEOUT;
}
