#ifndef HOOKANY64_SIGSCAN_H
#define HOOKANY64_SIGSCAN_H

#include <string>

__declspec(dllexport) void* sigscan(const unsigned char* pattern, const char* mask);
__declspec(dllexport) void* sigscan(const std::string& pattern);

void init_sigscan();

#endif
