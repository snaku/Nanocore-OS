#include "terminal/shell.h"
#include "terminal/terminal.h"
#include "kernel/debug/assert.h"
#include "kernel/cpu.h"
#include "ncos/info.h"
#include "ncstd/string.h"
#include "ncstd/memory.h"
#include "ncstd/bool.h"
#include "ncstd/stdlib.h"

enum Cmd
{
    CMD_NONE,

    CMD_HELP,
    CMD_ECHO,
    CMD_CLEAR,
    CMD_DUMP,
    CMD_INFO,

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
static ncbool shellExecuteInfo(const char* args);

static const ShellCmd s_cmds[CMD_MAX] =
{
    {"", "", NULL},

    {"help", "Print the command list.\n", shellExecuteHelp},
    {"echo", "Print a string to the terminal.\n", shellExecuteEcho},
    {"clear", "Clear the terminal.\n", shellExecuteClear},
    {"dump", "Dump registers or 16 bytes starting at a memory address.\n", shellExecuteDump},
    {"info", "Print info of the system.\n", shellExecuteInfo}
};

static void shellExecute(uint32_t cmdId, const char* args)
{
    NCOS_ASSERT(cmdId != CMD_NONE && cmdId < CMD_MAX, "Invalid shell command");

    ShellCmdFunc execute = s_cmds[cmdId].executeFunc;
    NCOS_ASSERT(execute != NULL, "Command has no exec func");

    if (!execute(args))
    {
        terminalWrite("Error while trying to execute command: '");
        terminalWrite(s_cmds[cmdId].name);
        terminalWrite("' \n");
    }
}

void shellTryExecute(const char* cmdStr)
{
    const char* cmdEnd = cmdStr;
    while (*cmdEnd != '\0' &&
           *cmdEnd != ' ')
    {
        cmdEnd++;
    }

    uint32_t foundCmd = CMD_NONE;

    for (uint32_t i = CMD_HELP; i < CMD_MAX; i++)
    {
        size_t cmdLen = cmdEnd - cmdStr;
        const ShellCmd* cmd = &s_cmds[i];

        if (strlen(cmd->name) == cmdLen &&
            strncmp(cmdStr, cmd->name, cmdLen) == 0)
        {
            foundCmd = i;
            break;
        }
    }

    if (foundCmd == CMD_NONE)
    {
        terminalWrite("Unknown command.\n");
        return;
    }

    const char* args = NULL;
    if (*cmdEnd == ' ')
    {
        cmdEnd++;
        while (*cmdEnd == ' ')
        {
            cmdEnd++;
        }

        if (*cmdEnd != '\0')
        {
            args = cmdEnd;
        }
    }

    shellExecute(foundCmd, args);
}

static ncbool shellExecuteHelp(const char* args)
{
    if (args != NULL) // help cmd doesn't take any arguments
    {
        return false;
    }

    terminalWrite("COMMAND LIST:\n");

    for (uint32_t i = CMD_HELP; i < CMD_MAX; i++)
    {
        terminalWrite("'");
        terminalWrite(s_cmds[i].name);
        terminalWrite("': ");
        terminalWrite(s_cmds[i].descr); // descr already has '\n'
    }

    return true;
}

static inline ncbool shellHasArgs(const char* args)
{
    return args != NULL;
}

static ncbool shellExecuteEcho(const char* args)
{
    if (!shellHasArgs(args))
    {
        return false;
    }

    terminalWrite(args);
    terminalWrite("\n");

    return true;
}

static ncbool shellExecuteClear(const char* args)
{
    if (shellHasArgs(args))
    {
        return false;
    }

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
    char* end;
    uint64_t addr = strtoull(args, &end, 16);
    if (addr == 0 ||
        end == args ||
        *end != '\0')
    {
        return false;
    }

    // TODO: check if the address is valid

    uint8_t* ptr = (uint8_t*)addr;
    for (uint32_t i = 0; i < 16; i++)
    {
        terminalWriteHex(*ptr++, TERMINAL_HEX_NOPREFIX); // i don't care
        terminalWrite(" ");
    }

    return true;
}

static ncbool shellExecuteDump(const char* args)
{
    if (!shellHasArgs(args))
    {
        return false;
    }

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

static ncbool shellExecuteInfo(const char* args)
{
    if (shellHasArgs(args))
    {
        return false;
    }

    const NcosInfo* ncosInfo = ncosGetInfo();

    terminalWrite(ncosInfo->name);
    terminalWrite(" ");
    terminalWrite(ncosInfo->version);
    terminalWrite("\n");

    terminalWrite("Architecture: ");
    terminalWrite(ncosInfo->arch);
    terminalWrite("\n");

    const CpuInfo* cpuInfo = cpuGetInfo();

    terminalWrite("CPU: ");
    terminalWrite(cpuInfo->name);
    terminalWrite("\n");

    terminalWrite("CPU Vendor: ");
    terminalWrite(cpuInfo->vendor);
    terminalWrite("\n");

    return true;
}