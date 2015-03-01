#ifndef EXTENSIONS_RETURN_VALUE_H
#define EXTENSIONS_RETURN_VALUE_H

#include <cstdio>

inline int error(int code, const char* message)
{
    printf("Error: %s (code: %d)\n", message, code);
    return code;
}

inline int success(int code, const char* message)
{
    printf("Success: %s (code: %d)\n", message, code);
    return code;
}

inline int success()
{
    return 0;
}

#endif // EXTENSIONS_RETURN_VALUE_H
