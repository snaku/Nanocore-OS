#pragma once

enum VgaColor
{
    VGA_COLOR_RED = 0x04,
    VGA_COLOR_DEFAULT = 0x07,
    VGA_COLOR_GREEN = 0x0a,
    VGA_COLOR_WHITE = 0x0f,

    VGA_COLOR_MAX = 0x100
};

void vgaPutc(char c, int color);
void vgaPuts(const char* str, int color);
void vgaPuti(int val, int color);

void vgaClear();