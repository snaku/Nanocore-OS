#include "ncos/info.h"

#define NCOS_NAME    "ncOS (Nanocore-OS)"
#define NCOS_VERSION "0.0.1"
#define NCOS_ARCH    "x86_64"

static const NcosInfo s_info =
{
    .name = NCOS_NAME,
    .version = NCOS_VERSION,
    .arch = NCOS_ARCH
};

const NcosInfo* ncosGetInfo()
{
    return &s_info;
}