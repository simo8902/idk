//
// Created by Simeon on 2/8/2025.
//

#ifndef LIBDATA_H
#define LIBDATA_H

#ifdef _WIN32
#include <Windows.h>
#endif

#include <cstdio>
#include <cstdlib>

#ifdef _WIN32
    #ifdef LIBDATA_EXPORTS
      #define LIBDATA_API __declspec(dllexport)
    #else
      #define LIBDATA_API __declspec(dllimport)
    #endif
#else
    #define LIBDATA_API
#endif

#define IDK_ASSERT(expr, msg)                       \
    do                                              \
    {                                               \
        if (!(expr))                                \
        {                                           \
            char buffer[512];                       \
            std::snprintf(buffer, sizeof(buffer),   \
            "*** IDK ERROR ***\n"                   \
                 "expression: %s\n"                 \
                 "file: %s\n"                       \
                 "line: %d\n"                       \
                 "%s\n",                            \
                 #expr, __FILE__, __LINE__, msg);   \
                 MessageBoxA(nullptr, buffer,       \
                 "Assertion Failed", MB_OK |        \
                 MB_ICONERROR);                     \
                 exit(EXIT_FAILURE);                \
        }                                           \
    }   while (0)

extern "C" LIBDATA_API void imguieffects();
extern "C" LIBDATA_API void hierarchyeffects();

#endif //LIBDATA_H