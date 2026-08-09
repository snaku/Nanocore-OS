#include "drivers/keyboard/keyboard.h"

// maps some ps/2 scancodes to KeyboardKeys
static const uint8_t s_irq1ToKey[128] =
{
    [0x01] = KEY_ESCAPE,

    [0x02] = KEY_1,
    [0x03] = KEY_2,
    [0x04] = KEY_3,
    [0x05] = KEY_4,
    [0x06] = KEY_5,
    [0x07] = KEY_6,
    [0x08] = KEY_7,
    [0x09] = KEY_8,
    [0x0a] = KEY_9,
    [0x0b] = KEY_0,

    [0x0e] = KEY_BACKSPACE,
    [0x0f] = KEY_TAB,

    [0x10] = KEY_Q,
    [0x11] = KEY_W,
    [0x12] = KEY_E,
    [0x13] = KEY_R,
    [0x14] = KEY_T,
    [0x15] = KEY_Y,
    [0x16] = KEY_U,
    [0x17] = KEY_I,
    [0x18] = KEY_O,
    [0x19] = KEY_P,

    [0x1c] = KEY_ENTER,
    [0x1d] = KEY_CTRL,
    [0x1e] = KEY_A,
    [0x1f] = KEY_S,
    [0x20] = KEY_D,
    [0x21] = KEY_F,
    [0x22] = KEY_G,
    [0x23] = KEY_H,
    [0x24] = KEY_J,
    [0x25] = KEY_K,
    [0x26] = KEY_L,

    [0x2a] = KEY_LSHIFT,
    [0x2c] = KEY_Z,
    [0x2d] = KEY_X,
    [0x2e] = KEY_C,
    [0x2f] = KEY_V,
    [0x30] = KEY_B,
    [0x31] = KEY_N,
    [0x32] = KEY_M,

    [0x36] = KEY_RSHIFT,
    [0x38] = KEY_ALT,
    [0x39] = KEY_SPACE,
    [0x3a] = KEY_CAPSLOCK
};

#define KEY_BUFF_MAX 16

typedef struct Keyboard
{
    uint8_t pressedKeys[KEY_BUFF_MAX];
    uint8_t releasedKeys[KEY_BUFF_MAX];
    uint8_t pressedKeyCount;
    uint8_t releasedKeyCount;
    ncbool capslock;
    ncbool shift;
} Keyboard;

static Keyboard s_kb;

static uint8_t kbIrq1ToKey(uint8_t scancode)
{
    uint8_t key = s_irq1ToKey[scancode];

    if (key >= KEY_MAX)
    {
        return KEY_INVALID;
    }

    return key;
}

static void kbUpdateModifier(uint8_t key, ncbool isPressed)
{
    switch (key)
    {
        case KEY_LSHIFT: // fallthrough
        case KEY_RSHIFT: s_kb.shift = isPressed; break;

        case KEY_CAPSLOCK:
            if (isPressed)
            {
                s_kb.capslock ^= true;
            }
            break;
    }
}

void kbReadScancode(uint8_t scancode)
{
    uint8_t key = kbIrq1ToKey(scancode % 0x80);
    if (key == KEY_INVALID)
    {
        return;
    }

    ncbool isPressed = !(scancode & 0x80); // if bit 7 is activated, then it's a key release

    if (isPressed &&
        s_kb.pressedKeyCount >= KEY_BUFF_MAX)
    {
        return;
    }
    else if (!isPressed && 
             s_kb.releasedKeyCount >= KEY_BUFF_MAX)
    {
        return;
    }

    if (isPressed)
    {
        s_kb.pressedKeys[s_kb.pressedKeyCount++] = key;
    }
    else
    {
        s_kb.releasedKeys[s_kb.releasedKeyCount++] = key;
    }

    kbUpdateModifier(key, isPressed);
}

void kbClearKeys()
{
    for (int i = 0; i < s_kb.pressedKeyCount; i++)
    {
        s_kb.pressedKeys[i] = KEY_INVALID;
    }

    for (int i = 0; i < s_kb.releasedKeyCount; i++)
    {
        s_kb.releasedKeys[i] = KEY_INVALID;
    }

    s_kb.pressedKeyCount = 0;
    s_kb.releasedKeyCount = 0;
}

uint8_t* kbGetPressedKeys()
{
    return s_kb.pressedKeys;
}

uint8_t kbGetPressedKeyCount()
{
    return s_kb.pressedKeyCount;
}

ncbool knCheckUppercase()
{
    return s_kb.capslock || s_kb.shift;
}