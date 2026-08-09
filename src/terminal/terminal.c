#include "terminal/terminal.h"
#include "drivers/vga/vga.h"
#include "drivers/keyboard/keyboard.h"
#include "ncstd/ctype.h"

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

static void terminalHandleEnter()
{
    switch (s_mode)
    {
        case TERMINAL_MODE_FREE: terminalWrite("\n"); break;
        case TERMINAL_MODE_CMD:
            // TODO
            return;
    }
}

static void terminalHandleSpecialKey(uint8_t key)
{
    switch (key)
    {
        case KEY_ENTER: terminalHandleEnter(); break;
        case KEY_SPACE: terminalWrite(" ");    break;
        case KEY_TAB:
            s_mode ^= TERMINAL_MODE_CMD;
            terminalWrite("\nCHANGING MODE...\n\n");
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
}

void terminalHandleInput()
{
    uint8_t* keys = kbGetPressedKeys();
    uint8_t keyCount = kbGetPressedKeyCount();

    for (int i = 0; i < keyCount; i++)
    {
        terminalHandleKey(keys[i]);
    }
}

void terminalWrite(const char* str)
{
    vgaPuts(str, VGA_COLOR_WHITE);
}

void terminalClear()
{
    vgaClear();
}