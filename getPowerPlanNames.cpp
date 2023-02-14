//
// Created by chris on 2/13/2023.
//

#include "getPowerPlanNames.h"
#include <iostream>
#include <vector>
#include <string>
#include <Windows.h>
extern "C" {
#include <Powrprof.h>
}
#pragma comment(lib, "Powrprof.lib")

using namespace std;

vector<string> getPowerPlanNames() {
    std::vector<string> plans;
    GUID buffer;
    DWORD bufferSize = sizeof(buffer);

    for (int index = 0;; index++) {
        bufferSize = sizeof(buffer);
        if (ERROR_SUCCESS == PowerEnumerate(nullptr, nullptr, &GUID_VIDEO_SUBGROUP, ACCESS_SCHEME, index, (UCHAR *) &buffer, &bufferSize)) {
            UCHAR displayBuffer[64] = { (UCHAR)0 };
            DWORD displayBufferSize = sizeof(displayBuffer);

            DWORD err = PowerReadFriendlyName(nullptr, &buffer, &NO_SUBGROUP_GUID, nullptr, (UCHAR *) &displayBuffer, &displayBufferSize);
            if (ERROR_SUCCESS == err) {
                wstring wide((wchar_t *) displayBuffer);
                string str( wide.begin(), wide.end() );
                plans.push_back(str);
            } else {
                if(ERROR_MORE_DATA == err) cout << "data" << err  << endl;
            }
        } else break;
    }

    return plans;
}