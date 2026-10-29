#include "terminal/terminal.h"
#include "terminal/shell.h"
#include "drivers/vga/vga.h"
#include "drivers/keyboard/keyboard.h"
#include "ncstd/ctype.h"
#include "ncstd/string.h"
#include "ncstd/stdarg.h"

static char s_keyToChar[37] =
{
    '\0', // KEY_INVALID

    'a', 'b', 'c', 'd', 'e',
    'f', 'g', 'h', 'i', 'j',
    'k', 'l', 'm', 'n', 'o',
    'p', 'q', 'r', 's', 't',
    'u', 'v', 'w', 'x', 'y',
    'z',

    '0', '1', '2', '3', '4',
    '5', '6', '7', '8', '9'
};

enum TerminalMode
{
    TERMINAL_MODE_FREE,
    TERMINAL_MODE_CMD
};


#define TERMINAL_CMDBUFF_MAX 128

static char s_cmdBuff[TERMINAL_CMDBUFF_MAX];
static uint32_t s_cmdBuffIdx;

static uint8_t s_mode;

static char terminalKeyToChar(uint8_t key)
{
    if (key == KEY_INVALID ||
        key > KEY_9)
    {
        return s_keyToChar[KEY_INVALID];
    }

    return s_keyToChar[key];
}

static inline void terminalWritePrompt()
{
    terminalWrite("ncOS> ");
}

static inline void terminalCmdBuffClear()
{
    s_cmdBuffIdx = 0;
    s_cmdBuff[0] = '\0';
}

static void terminalHandleEnter()
{
    terminalWrite("\n");

    switch (s_mode)
    {
        case TERMINAL_MODE_FREE: break;
        case TERMINAL_MODE_CMD:
            shellTryExecute(s_cmdBuff);
            terminalCmdBuffClear();

            terminalWritePrompt();
            return;
    }
}

static void terminalBackspace()
{
    vgaBackspace();
}

static void terminalHandleBackspace()
{
    if (s_mode == TERMINAL_MODE_CMD)
    {
        if (s_cmdBuffIdx == 0)
        {
            return;
        }

        s_cmdBuff[--s_cmdBuffIdx] = '\0';
    }

    terminalBackspace();
}

static void terminalAddToCmdBuff(char c)
{
    if (s_cmdBuffIdx >= TERMINAL_CMDBUFF_MAX - 1) // - 1 for '\0'
    {
        return;
    }

    s_cmdBuff[s_cmdBuffIdx++] = c;
    s_cmdBuff[s_cmdBuffIdx] = '\0';
}

static void terminalHandleSpecialKey(uint8_t key)
{
    switch (key)
    {
        case KEY_ENTER: terminalHandleEnter(); break;
        case KEY_BACKSPACE: terminalHandleBackspace(); break;

        case KEY_SPACE:
            if (s_mode == TERMINAL_MODE_CMD)
            {
                terminalAddToCmdBuff(' ');
            }

            terminalWrite(" ");
            break;

        case KEY_TAB:
            s_mode ^= TERMINAL_MODE_CMD;

            terminalClear();

            if (s_mode == TERMINAL_MODE_CMD)
            {
                terminalWritePrompt();
            }
            break;
    }
}

static void terminalHandleKey(uint8_t key)
{
    char character[2];

    character[0] = terminalKeyToChar(key);
    character[1] = '\0';

    if (character[0] != '\0')
    {
        if (kbCheckUppercase())
        {
            character[0] = toupper(character[0]);
        }

        if (s_mode == TERMINAL_MODE_CMD)
        {
            if (s_cmdBuffIdx >= TERMINAL_CMDBUFF_MAX - 1)
            {
                return;
            }

            terminalAddToCmdBuff(character[0]);
        }

        terminalWrite(character);
    }
    else
    {
        terminalHandleSpecialKey(key);
    }
}

void terminalInit()
{
    terminalClear();
    terminalWrite("Press TAB to change mode.\n");

    s_mode = TERMINAL_MODE_CMD;
    terminalWritePrompt();
}

void terminalHandleInput()
{
    uint8_t keys[KEY_BUFF_MAX];
    uint8_t keyCount = kbConsumePressedKeys(keys);

    for (int i = 0; i < keyCount; i++)
    {
        terminalHandleKey(keys[i]);
    }
}

void terminalWrite(const char* str)
{
    vgaPuts(str, VGA_COLOR_WHITE);
}

void terminalWriteHex(int64_t hex, uint8_t prefix)
{
    vgaPuthex(hex, VGA_COLOR_WHITE, prefix == TERMINAL_HEX_PREFIX);
}

void terminalWriteFmt(const char* fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vgaPutFmtV(VGA_COLOR_WHITE, fmt, args);
    va_end(args);
}

void terminalClear()
{
    terminalCmdBuffClear();
    vgaClear();
}