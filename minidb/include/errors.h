#ifndef ERRORS_H
#define ERRORS_H

typedef enum
{
    OK = 0,
    ERR_UNKNOWN,
    ERR_IO,
    ERR_UNSUPPORTED_FORMAT,
    ERR_FULL,
    ERR_PARSE,
    ERR_INVALID,
    ERR_UNSUPPORTED,
} ErrorCode;

#endif
