#pragma once

#include <string>
#include <cstring>
#include <iostream>

enum class Owner {
    FORWARD = 1,
    DEFENDER = 2,
    GOALKEEPER = 3,
    DOCTOR = 4,
    FIELD = 5,
    COACH = 6
};

enum class State {
    // ������ �� �����
    NONE,
    // ��������� � ����� ��������� ������ ��� ���������
    INIT_DATA,
    // �������� ������ ��� ���������
    GET_DATA,
    // ��������� � ����� ��������� ���������
    SET_RESULT,
    // �������� �� ������ ��������� ���������
    GET_RESULT
};

class Message {
public:
    Owner sender;
    int data;
    State state;

    Message(Owner pSender, int pData, State pState = State::NONE) {
        sender = pSender;
        data = pData;
        state = pState;
    }

    Message(void *buffer) {
        std::memcpy(&sender, buffer, sizeof(int));
        std::memcpy(&data, ((int *) buffer) + 1 * sizeof(int), sizeof(int));
        std::memcpy(&state, ((int *) buffer) + 2 * sizeof(int), sizeof(int));
    }

    Message(int *buffer) {
        std::memcpy(&sender, buffer, sizeof(int));
        std::memcpy(&data, buffer + 1 * sizeof(int), sizeof(int));
        std::memcpy(&state, buffer + 2 * sizeof(int), sizeof(int));
    }

    ~Message() = default;

    void write(void *buffer) {
        std::memcpy(buffer, &sender, sizeof(int));
        std::memcpy(((int *) buffer) + 1 * sizeof(int), &data, sizeof(int));
        std::memcpy(((int *) buffer) + 2 * sizeof(int), &state, sizeof(int));
    }

    void write(int *buffer) {
        std::memcpy(buffer, &sender, sizeof(int));
        std::memcpy((buffer) + 1 * sizeof(int), &data, sizeof(int));
        std::memcpy((buffer) + 2 * sizeof(int), &state, sizeof(int));
    }

    std::string toString() const {
        return "Owner = " + std::to_string(static_cast<int>(sender)) + ", data = " + std::to_string(data) +
               " state = " + std::to_string(static_cast<int>(state)) + "\n";
    }

    std::wstring toWstring() const {
        return L"Owner = " + std::to_wstring(static_cast<int>(sender)) + L", data = " + std::to_wstring(data) +
               L" state = " + std::to_wstring(static_cast<int>(state)) + L"\n";
    }

};

