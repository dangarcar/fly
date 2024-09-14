#pragma once

#include <cstdarg>
#include <cstdio>

void writeLog(const char* format, ...) {
    va_list args;
    va_start(args, format);

    vprintf(format, args);

    va_end(args);
}

void writeError(const char* format, ...) {
    va_list args;
    va_start(args, format);

    vfprintf(stderr, format, args);

    va_end(args);
}