#pragma once

typedef enum {
    ALLOCATION_ERROR,
    INPUT_ERROR
} ErrorType;

void PrintAndExitWithError(const char* format, ...);
void ExitWithError(ErrorType error);
