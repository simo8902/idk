//
// Created by Simeon on 10/11/2024.
//

#ifndef TRACKER_H
#define TRACKER_H

#include <Windows.h>

inline unsigned int GetNumberOfLogicalCores() {
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    return sysInfo.dwNumberOfProcessors;  // Returns number of logical cores
}

inline double GetProcessCPUTime() {
    FILETIME creationTime, exitTime, kernelTime, userTime;
    if (GetProcessTimes(GetCurrentProcess(), &creationTime, &exitTime, &kernelTime, &userTime)) {
        ULARGE_INTEGER uKernelTime, uUserTime;
        uKernelTime.LowPart = kernelTime.dwLowDateTime;
        uKernelTime.HighPart = kernelTime.dwHighDateTime;
        uUserTime.LowPart = userTime.dwLowDateTime;
        uUserTime.HighPart = userTime.dwHighDateTime;

        return (double)(uUserTime.QuadPart + uKernelTime.QuadPart) / 10000000.0;  // Time in seconds
    }
    return 0.0;
}

// Get the total CPU usage as a percentage of all logical cores
inline double GetProcessCPUUsage(double elapsedTime) {
    static double lastTotalCPUTime = 0.0;
    double currentTotalCPUTime = GetProcessCPUTime();
    double cpuTimeDifference = currentTotalCPUTime - lastTotalCPUTime;
    lastTotalCPUTime = currentTotalCPUTime;

    unsigned int numLogicalCores = GetNumberOfLogicalCores();
    return (cpuTimeDifference / (elapsedTime * numLogicalCores)) * 100.0;
}

#endif //TRACKER_H
