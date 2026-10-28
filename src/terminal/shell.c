#include "terminal/shell.h"
#include "terminal/terminal.h"
#include "kernel/debug/assert.h"
#include "kernel/cpu.h"
#include "ncos/info.h"
#include "ncstd/string.h"
#include "ncstd/memory.h"
#include "ncstd/bool.h"
#include "ncstd/stdlib.h"
#include "ncstd/ctype.h"

enum Cmd
{
    CMD_NONE,

    CMD_HELP,
    CMD_ECHO,
    CMD_CLEAR,
    CMD_DUMP,
    CMD_POKE,
    CMD_INFO,
    CMD_REBOOT,
    CMD_REPEAT,

    CMD_MAX
};

typedef ncbool (*ShellCmdFunc)(const char* args);

typedef struct ShellCmd
{
    const char* name;
    const char* descr;
    ShellCmdFunc executeFunc;
} ShellCmd;

static ncbool shellExecuteHelp(const char* args);
static ncbool shellExecuteEcho(const char* args);
static ncbool shellExecuteClear(const char* args);
static ncbool shellExecuteDump(const char* args);
static ncbool shellExecutePoke(const char* args);
static ncbool shellExecuteInfo(const char* args);
static ncbool shellExecuteReboot(const char* args);
static ncbool shellExecuteRepeat(const char* args);

static const ShellCmd s_cmds[CMD_MAX] =
{
    {"", "", NULL},

    {"help", "Print the command list.", shellExecuteHelp},
    {"echo", "Print a string to the terminal.", shellExecuteEcho},
    {"clear", "Clear the terminal.", shellExecuteClear},
    {"dump", "Dump registers or 1 to 256 bytes starting at a memory address.", shellExecuteDump},
    {"poke", "Write a byte at a memory address.", shellExecutePoke},
    {"info", "Print info of the system.", shellExecuteInfo},
    {"reboot", "Reboot the system.", shellExecuteReboot},
    {"repeat", "Repeat a command n times (max = 16 times).", shellExecuteRepeat},
};

#define SHELL_REQUIRE_ARGS(args) \
    if (!shellHasArgs(args))     \
    {                            \
        return false;            \
    }

#define SHELL_REQUIRE_NOARGS(args) \
    if (shellHasArgs(args))        \
    {                              \
        return false;              \
    }

#define SHELL_DUMPADDR_MAX_COUNT 256

#define SHELL_REPEAT_MAX 16
#define SHELL_REPEAT_MAX_DEPTH 2

static void shellExecute(uint32_t cmdId, const char* args)
{
    NCOS_ASSERT(cmdId != CMD_NONE && cmdId < CMD_MAX, "Invalid shell command");

    ShellCmdFunc execute = s_cmds[cmdId].executeFunc;
    NCOS_ASSERT(execute != NULL, "Command has no exec func");

    if (!execute(args))
    {
        terminalWriteFmt("Error while trying to execute command '%s'.\n", s_cmds[cmdId].name);
    }
}

void shellTryExecute(const char* cmdStr)
{
    while (isspace(*cmdStr))
    {
        cmdStr++;
    }

    if (*cmdStr == '\0')
    {
        return;
    }

    const char* cmdEnd = cmdStr;
    while (*cmdEnd != '\0' &&
           !isspace(*cmdEnd))
    {
        cmdEnd++;
    }

    uint32_t foundCmd = CMD_NONE;
    size_t cmdLen = cmdEnd - cmdStr;
    for (uint32_t i = CMD_HELP; i < CMD_MAX; i++)
    {
        const ShellCmd* cmd = &s_cmds[i];

        if (strlen(cmd->name) == cmdLen &&
            strncmp(cmdStr, cmd->name, cmdLen) == 0)
        {
            foundCmd = i;
            break;
        }
    }

    if (foundCmd != CMD_NONE)
    {
        while (isspace(*cmdEnd))
        {
            cmdEnd++;
        }

        const char* args = NULL;
        if (*cmdEnd != '\0')
        {
            args = cmdEnd;
        }

        shellExecute(foundCmd, args);
    }
    else
    {
        char cmd[cmdLen + 1];
        memcpy(cmd, cmdStr, cmdLen);
        cmd[cmdLen] = '\0';

        terminalWriteFmt("Unknown command '%s'.\n", cmd);
    }

    if (foundCmd != CMD_CLEAR)
    {
        terminalWrite("\n");
    }
}

static inline ncbool shellHasArgs(const char* args)
{
    return args != NULL;
}

static ncbool shellFindArg(const char* args, uint8_t idx, char* buff, size_t buffLen)
{
    if (args == NULL || 
        buff == NULL ||
        buffLen == 0)
    {
        return false;
    }

    buff[0] = '\0';

    for (uint32_t i = 0; i <= idx; i++)
    {
        while (*args != '\0' && 
               isspace(*args))
        {
            args++;
        }

        if (i == idx)
        {
            uint32_t buffIdx = 0;
            while (*args != '\0' && 
                   !isspace(*args))
            {
                if (buffIdx >= buffLen - 1)
                {
                    return false;
                }

                buff[buffIdx++] = *args++;
            }

            buff[buffIdx] = '\0';

            return true;
        }

        while (*args != '\0' &&
               !isspace(*args))
        {
            args++;
        }
    }

    return false;
}

static ncbool shellExecuteHelp(const char* args)
{
    SHELL_REQUIRE_NOARGS(args);

    terminalWrite("COMMAND LIST:\n");

    for (uint32_t i = CMD_HELP; i < CMD_MAX; i++)
    {
        const ShellCmd* cmd = &s_cmds[i];
        terminalWriteFmt("'%s': %s\n", cmd->name, cmd->descr);
    }

    return true;
}

static ncbool shellExecuteEcho(const char* args)
{
    SHELL_REQUIRE_ARGS(args);

    terminalWriteFmt("%s\n", args);

    return true;
}

static ncbool shellExecuteClear(const char* args)
{
    SHELL_REQUIRE_NOARGS(args);

    terminalClear();

    return true;
}

static ncbool shellDumpRegs()
{
    const Registers* regs = cpuGetRegs();
    for (uint8_t i = REG_RAX; i < REG_MAX; i++)
    {
        terminalWrite(cpuRegToStr(i));
        terminalWrite(": ");
        terminalWriteHex(regs->vals[i], TERMINAL_HEX_PREFIX);
        terminalWrite("\n");
    }

    return true;
}

static ncbool shellDumpMemory(const char* args)
{
    char addrBuff[17];
    if (!shellFindArg(args, 0, addrBuff, sizeof(addrBuff)))
    {
        return false;
    }

    char* endAddr;
    uint64_t addr = strtoull(addrBuff, &endAddr, 16);
    if (*endAddr != '\0' ||
        addr == 0) // TODO: check if the address is valid
    {
        return false;
    }

    char countBuff[4];
    if (!shellFindArg(args, 1, countBuff, sizeof(countBuff)))
    {
        return false;
    }

    char* endCount;
    uint64_t count = strtoull(countBuff, &endCount, 10);
    if (*endCount != '\0' ||
        count == 0 ||
        count > SHELL_DUMPADDR_MAX_COUNT)
    {
        return false;
    }

    uint8_t* ptr = (uint8_t*)addr;
    for (uint64_t i = 0; i < count; i += 16)
    {
        terminalWriteFmt("%x: ", addr + i);

        for (uint64_t j = 0; j < 16 && i + j < count; j++)
        {
            terminalWriteHex(ptr[i + j], TERMINAL_HEX_NOPREFIX);
            terminalWrite(" ");
        }

        terminalWrite("|");

        for (uint64_t j = 0; j < 16 && i + j < count; j++)
        {
            char c = ptr[i + j];
            if (isprint(c))
            {
                terminalWriteFmt("%c", c);
            }
            else
            {
                terminalWrite(".");
            }
        }

        terminalWrite("|\n");
    }

    return true;
}

static ncbool shellExecuteDump(const char* args)
{
    SHELL_REQUIRE_ARGS(args);

    if (strcmp(args, "regs") == 0)
    {
        return shellDumpRegs();
    }

    if (!shellDumpMemory(args))
    {
        return false;
    }

    terminalWrite("\n");

    return true;
}

static ncbool shellExecutePoke(const char* args)
{
    SHELL_REQUIRE_ARGS(args);

    char addrBuff[17];
    if (!shellFindArg(args, 0, addrBuff, sizeof(addrBuff)))
    {
        return false;
    }

    char* endAddr;
    uint64_t addr = strtoull(addrBuff, &endAddr, 16);
    if (*endAddr != '\0' ||
        addr == 0) // TODO: check if the address is valid
    {
        return false;
    }

    char valBuff[17];
    if (!shellFindArg(args, 1, valBuff, sizeof(valBuff)))
    {
        return false;
    }

    char* endVal;
    uint64_t val = strtoull(valBuff, &endVal, 16);
    if (*endVal != '\0' ||
        val > 0xff)
    {
        return false;
    }

    uint8_t* ptr = (uint8_t*)addr;
    uint8_t oldVal = *ptr;

    *ptr = val;

    terminalWriteFmt("%x: Old value -> %x || New value -> %x\n", addr, oldVal, val);

    return true;
}

static ncbool shellExecuteInfo(const char* args)
{
    SHELL_REQUIRE_NOARGS(args);

    const NcosInfo* ncosInfo = ncosGetInfo();
    terminalWriteFmt("%s %s\n", ncosInfo->name, ncosInfo->version);
    terminalWriteFmt("Architecture: %s\n", ncosInfo->arch);

    const CpuInfo* cpuInfo = cpuGetInfo();
    terminalWriteFmt("CPU: %s\n", cpuInfo->name);
    terminalWriteFmt("CPU Vendor: %s\n", cpuInfo->vendor);

    return true;
}

static ncbool shellExecuteReboot(const char* args)
{
    SHELL_REQUIRE_NOARGS(args);

    OUTB(0x64, 0xfe);

    return true;
}

static ncbool shellExecuteRepeat(const char* args)
{
    static uint32_t depth = 0;

    SHELL_REQUIRE_ARGS(args);

    if (depth >= SHELL_REPEAT_MAX_DEPTH)
    {
        return false;
    }

    char countBuff[8];
    if (!shellFindArg(args, 0, countBuff, sizeof(countBuff)))
    {
        return false;
    }

    char* end;
    uint64_t count = strtoull(countBuff, &end, 10);
    if (*end != '\0' ||
        count == 0 ||
        count > SHELL_REPEAT_MAX)
    {
        return false;
    }

    // find command to repeat
    while (isdigit(*args))
    {
        args++;

        if (*args == '\0')
        {
            return false;
        }
    }

    depth++;

    for (uint64_t i = 0; i < count; i++)
    {
        shellTryExecute(args);
    }

    depth--;

    return true;
}