#pragma once

#include "ncstd/types.h"

enum TerminalHexPrefix
{
    TERMINAL_HEX_NOPREFIX,
    TERMINAL_HEX_PREFIX
};

void terminalInit();
void terminalHandleInput();
void terminalWrite(const char* str);
void terminalWriteHex(int64_t hex, uint8_t prefix);
void terminalClear();