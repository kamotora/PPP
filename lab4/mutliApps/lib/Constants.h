#pragma once
#include <iostream>

using namespace std;

static const wstring END_GAME_SEMAPHORE = L"end_game";
static const wstring COMMAND_COACH_SEMAPHORE = L"COMMAND_SEMAPHORE";
static const wstring GET_BALL_SEMAPHORE = L"GET_BALL_SEMAPHORE";
static const wstring GET_BALL_CHANNEL = L"GET_BALL_CH";
static const wstring RETURN_BALL_CHANNEL = L"RETURN_BALL_CH";
static const wstring COACH_CHANNEL = L"COACH_CHANNEL";
static const wstring FORWARD_FIELD_CHANNEL = L"FORW_FIELD_CHANNEL";
static const wstring ANSWER_FORWARD_CHANNEL = L"ANSW_FORW_CHANNEL";
static const wstring DOCTOR_REQUEST_CHANNEL = L"DOC1_CHANNEL";
static const wstring DOCTOR_RESPONSE_CHANNEL = L"DOC2_CHANNEL";
static const wstring FORWARD_DEFENDER_RENDEZVOUS_CH1 = L"REND_1_CH1";
static const wstring FORWARD_DEFENDER_RENDEZVOUS_CH2 = L"REND_1_CH2";
static const wstring FIELD_GOALKEEPER_RENDEZVOUS_CH1 = L"REND_2_CH1";
static const wstring FIELD_GOALKEEPER_RENDEZVOUS_CH2 = L"REND_2_CH2";
static const int MAX_WAIT_TIME = 10000;
static const int SIGNAL_WAIT_TIME = 3000;
static const int FILE_SIZE = 4096;
//Максимальный уровень сил
static const int MAX_POWER = 100;
//Максимальная потеря сил за 1 действие
static const int MAX_POWER_DIFF = 30;
//Максимальный уровень мастерства
static const int MAX_SKILL = 100;
static const int COUNT_GAMES = 10;
// Пауза между играми
static const int PAUSE_BETWEEN_GAMES = 1000;

static const int FORWARD_TIRED_MSG = 1;
static const int DEFENDER_TIRED_MSG = 2;
static const int GOALKEEPER_TIRED_MSG = 3;
static const int FORWARD_RELAX_MSG = -1;
static const int DEFENDER_RELAX_MSG = -2;
static const int GOALKEEPER_RELAX_MSG = -3;

static const int IS_GOAL_MSG = 1;
static const int IS_NOT_GOAL_MSG = -1;

static const int GOOD_FORWARD_MSG = 1;
static const int BAD_FORWARD_MSG = -1;

static const int FORWARD_SAVE_BALL = 1;
static const int FORWARD_NOT_SAVE_BALL = -1;

// Признак передачи мяча от одного участника игры другому
static const int BALL_MSG = 10;
typedef void (*functionType)();