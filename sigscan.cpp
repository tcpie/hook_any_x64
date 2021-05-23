#include "sigscan.h"

#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>

#include <windows.h>
#include <winbase.h>
#include <winnt.h>
#include <Psapi.h>
#include <winternl.h>
#include <stdint.h>
#include <iostream>

uint64_t search_start = 0;
DWORD search_size = 0;

void init_sigscan()
{
    HANDLE currProcess = GetCurrentProcess();
    MODULEINFO modInfo = { NULL, };
    GetModuleInformation(currProcess, GetModuleHandleA("GTA5.exe"), &modInfo, sizeof(modInfo));

    DWORD dummyDword = NULL;

    search_start = (uint64_t)modInfo.lpBaseOfDll;
    search_size = modInfo.SizeOfImage;
}

void* sigscan(const std::string& pattern)
{
    std::istringstream iss(pattern);
    std::vector<std::string> tokens{ std::istream_iterator<std::string>{iss},
                          std::istream_iterator<std::string>{} };

    std::vector<uint8_t> vals;
    std::vector<char> mask;

    for (auto str : tokens)
    {
        if (str == "??") {
            vals.push_back(1);
            mask.push_back('?');
            continue;
        }

        uint8_t val = std::stoul(str, nullptr, 16);

        vals.push_back(val);
        mask.push_back('x');
    }

    // NULL terminate our mask
    mask.push_back('\0');

    return sigscan(vals.data(), mask.data());
}

void* sigscan(const unsigned char* pattern, const char* mask)
{
    if (search_start == 0 || search_size == 0) {
        init_sigscan();
    }

    unsigned int patternLength = strlen(mask);
 
    for (uint64_t i = 0; i < search_size - patternLength; i++)
    {
        bool found = true;
        for (uint64_t j = 0; j < patternLength; j++)
        {
            if (mask[j] != '?' && pattern[j] != *(uint8_t*)(search_start + i + j))
            {
                found = false;
                break;
            }
        }
        if (found)
        {
            return (char*)(search_start + i);
        }
    }

    return nullptr;
}