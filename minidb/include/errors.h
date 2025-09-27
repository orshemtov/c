#ifndef ERRORS_H
#define ERRORS_H

typedef enum
{
    ERR_OK = 0,
    ERR_UNKNOWN,
    ERR_IO,
    ERR_UNSUPPORTED_FORMAT,
    ERR_FULL,
    ERR_INVALID,
} ErrorCode;

#endif