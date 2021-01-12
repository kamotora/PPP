#include <iostream>
#include "windows.h"
#include <ctime>

using namespace std;

LPCSTR _s2ws(const string& s);
LPCWSTR s2ws(const string& s);
string ws2s(LPCWSTR s);
int countChance(int power, int skill);