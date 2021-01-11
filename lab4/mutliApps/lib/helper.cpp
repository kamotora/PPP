#include "helper.h"


LPCWSTR s2ws(const string& s)
{
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    std::wstring r(buf);
    delete[] buf;
    return r.c_str();
}

string ws2s(LPCWSTR s)
{
    wstring ws(s);
    return string(ws.begin(), ws.end());
}

int countChance(int power, int skill) {
    return (power + skill) / 2;
}

LPCSTR _s2ws(const string& s)
{
    return reinterpret_cast<LPCSTR>(s.c_str());
}