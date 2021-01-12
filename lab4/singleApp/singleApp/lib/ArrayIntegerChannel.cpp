#include "ArrayIntegerChannel.h"

ArrayIntegerChannel::ArrayIntegerChannel(const wstring &name, functionType terminateFunc) {
    const wstring nameFreeSemaphore = L"free_" + name;
    const wstring nameEmptySemaphore = L"empty_" + name;
    const wstring fileName = L"file_" + name;
    this->name = name;
    this->emptySemaphore = new BinarySemaphore(nameEmptySemaphore, 1);
    this->endGameSignal = new Signal();

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

ArrayIntegerChannel::~ArrayIntegerChannel() {
    wcout << "Array Channel " << name << " start deleted\n";
    endGameSignal->setSignal();
    delete emptySemaphore;
    delete endGameSignal;
    UnmapViewOfFile(buffer);
    CloseHandle(this->fileMem);
}

void ArrayIntegerChannel::setData(Message *data, int timeout) {
    while (!this->emptySemaphore->close(timeout))
    {
        wcout << "Channel " << name << " timeout end, check is game ended\n";
        if(endGameSignal->isSignal())
            terminateFunc();
    }
    addMessage(data);
//    wcout << L"Data " << name << L" setted: " << data->toWstring() << endl;
    this->emptySemaphore->open();
}

Message *ArrayIntegerChannel::getData(Owner receiver, int timeout) {
    Message *message;
    do {
        while (!this->emptySemaphore->close(timeout))
        {
            wcout << "Channel " << name << " timeout end, check is game ended\n";
            if(endGameSignal->isSignal())
                terminateFunc();
        }
        message = findMessageInBuffer(receiver);
        this->emptySemaphore->open();
        if (message == nullptr)
            Sleep(300);
    } while (message == nullptr);
//    wcout << L"Data " << name << L" getted: " << message->toWstring() << endl;
    return message;
}

Message *ArrayIntegerChannel::findMessageInBuffer(Owner receiver) {
    Message *result = nullptr;
    vector<Message *> messages = getMessages();
    for (int i = 0; i < messages.size(); i++)
        if (messages[i]->sender == receiver) {
            result = messages[i];
            messages.erase(messages.begin() + i);
            break;
        }
    writeMessages(messages);
    return result;
}

BOOL ArrayIntegerChannel::writeMessages(vector<Message *> messages) {
    int size = messages.size() * sizeof(Message) + sizeof(DWORD);
    if (size > FILE_SIZE) {
        wcerr << L"size of array in " << name << L" very large\n";
        throw std::exception("error");
    }

    int messagesCount = messages.size();
    std::memcpy(buffer, &messagesCount, sizeof(int));
    auto ptrForArray = ((int *) buffer) + 1 * sizeof(int);
    for (int i = 0; i < messagesCount; i++) {
        messages[i]->write(ptrForArray);
        ptrForArray += sizeof(Message);
    }
    return TRUE;
}

vector<Message *> ArrayIntegerChannel::getMessages() {
    vector<Message *> result;
    int messagesCount;
    std::memcpy(&messagesCount, buffer, sizeof(int));
    if (messagesCount == 0)
        return result;
    auto ptrForArray = ((int *) buffer) + 1 * sizeof(int);
    for (int i = 0; i < messagesCount; i++) {
        result.push_back(new Message(ptrForArray));
        ptrForArray += sizeof(Message);
    }
    return result;
}

bool ArrayIntegerChannel::addMessage(Message *data) {
    auto messages = getMessages();
    messages.emplace_back(data);
    writeMessages(messages);
    return true;
}

