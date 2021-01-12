#include "BinarySemaphore.h"

BinarySemaphore::BinarySemaphore(const wstring &name, int startState, functionType termFunc) {
    this->terminateFunc = termFunc;
    this->endGameSignal = new Signal();
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
void BinarySemaphore::open() {
        while (ReleaseSemaphore(sem, 1, NULL) == WAIT_TIMEOUT)
        {
            wcout << "Semaphore " << name << " timeout end in open, check is game ended\n";
            if(endGameSignal->isSignal())
                terminateFunc();
        }
}

// P()
void BinarySemaphore::close(DWORD milliseconds) {
    while (WaitForSingleObject(sem, milliseconds) == WAIT_TIMEOUT)
    {
            wcout << "Semaphore " << name << " timeout end in close, check is game ended\n";
        if(endGameSignal->isSignal())
            terminateFunc();
    }
}
