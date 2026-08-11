#pragma once

typedef struct NcosInfo
{
    const char* name;
    const char* version;
    const char* arch;
} NcosInfo;

const NcosInfo* ncosGetInfo();