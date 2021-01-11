#include "IntegerChannel.h"


IntegerChannel::IntegerChannel(const wstring &name) {
    const wstring nameFreeSemaphore = L"free_" + name;
    const wstring nameEmptySemaphore = L"empty_" + name;
    const wstring fileName = L"file_" + name;
    this->name = name;
    this->freeSemaphore = new BinarySemaphore(nameFreeSemaphore, 1);
    this->emptySemaphore = new BinarySemaphore(nameEmptySemaphore);


    this->fileMem = OpenFileMappingW(FILE_MAP_ALL_ACCESS, false, fileName.c_str());
    if (this->fileMem == NULL) {
        this->fileMem = CreateFileMappingW(
                INVALID_HANDLE_VALUE,
                nullptr, PAGE_READWRITE,
                0, FILE_SIZE,
                fileName.c_str());
    }

    if (this->fileMem != NULL) {
        this->buffer = MapViewOfFile(
                fileMem,
                FILE_MAP_ALL_ACCESS,
                0, 0,
                FILE_SIZE);
        if (buffer == NULL) {
            printf("Could not map view of file (%lu).\n", GetLastError());
            CloseHandle(fileMem);
        }
    } else {
        printf("Could not open file mapping object (%lu).\n", GetLastError());
        buffer = NULL;
    }
}

IntegerChannel::~IntegerChannel() {
    delete freeSemaphore;
    delete emptySemaphore;
    UnmapViewOfFile(this->buffer);
    CloseHandle(this->fileMem);
}

void IntegerChannel::setData(Message *data, int timeout) {
    // Если ждём слишком долго, что-то пошло не так и завершаем работу
    if(!this->freeSemaphore->close(timeout))
        ExitThread(0);
    data->write(buffer);
//    wcout << L"Data " << name << L" setted: " << data->toWstring() << endl;
    this->emptySemaphore->open();
}

Message *IntegerChannel::getData(int timeout) {
    Message *message;
    if(!this->emptySemaphore->close(timeout))
        ExitThread(0);
    message = new Message(buffer);
//    wcout << L"Data " << name << L" getted: " << message->toWstring() << endl;
    this->freeSemaphore->open();
    return message;
}


