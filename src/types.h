#ifndef TYPES_H
#define TYPES_H

#include <cstdint>
#include <limits.h>

typedef long ptr;
#define PTR_NULL -1
#define LEVEL_END UINT_MAX-1
#define JOB_END UINT_MAX-2

// To be used in return codes for hashtable and table
enum ResultCode{FOUND, NOT_FOUND};
#endif
