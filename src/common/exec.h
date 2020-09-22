#ifndef _EXEC_H_
#define _EXEC_H_

#include "common/util.h"

bool exec(const char* file, char* const* argv, char* out, int out_len);

#endif