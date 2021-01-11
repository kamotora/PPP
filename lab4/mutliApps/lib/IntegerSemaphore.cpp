#include "IntegerSemaphore.h"

IntegerSemaphore::IntegerSemaphore(const wstring &name, int startState, int maxState) {
    sem = OpenSemaphoreW(SEMAPHORE_ALL_ACCESS, true, name.c_str());
    if (sem == NULL) {
        sem = CreateSemaphoreW(NULL, startState, maxState, name.c_str());
    }
}

IntegerSemaphore::~IntegerSemaphore() {
    CloseHandle(sem);
}

// V()
bool IntegerSemaphore::open(int n) {
    DWORD res = ReleaseSemaphore(sem, n, NULL);
    return res == WAIT_TIMEOUT;
}
// P()
bool IntegerSemaphore::close(DWORD milliseconds) {
    DWORD result = WaitForSingleObject(sem, milliseconds);
    return result != WAIT_TIMEOUT;
}

