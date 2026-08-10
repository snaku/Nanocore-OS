#pragma once

#include "ncstd/types.h"

void terminalInit();
void terminalHandleInput();
void terminalWrite(const char* str);
void terminalWriteHex(int64_t hex);
void terminalClear();