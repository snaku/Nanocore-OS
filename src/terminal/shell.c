#include "terminal/shell.h"
#include "terminal/terminal.h"
#include "kernel/debug/assert.h"
#include "kernel/cpu.h"
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

static const ShellCmd s_cmds[CMD_MAX] =
{
    {"", "", NULL},

    {"help", "Print the command list.\n", shellExecuteHelp},
    {"echo", "Print a string to the terminal.\n", shellExecuteEcho},
    {"clear", "Clear the terminal.\n", shellExecuteClear},
    {"dump", "Dump registers or memory.\n", shellExecuteDump}
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

static ncbool shellExecuteEcho(const char* args)
{
    if (args == NULL)
    {
        return false;
    }

    terminalWrite(args);
    terminalWrite("\n");

    return true;
}

static ncbool shellExecuteClear(const char* args)
{
    if (args != NULL)
    {
        return false;
    }

    terminalClear();

    return true;
}

static ncbool shellExecuteDump(const char* args)
{
    if (args == NULL)
    {
        return false;
    }

    if (strcmp(args, "regs") == 0)
    {
        const Registers* regs = cpuGetRegs();
        for (uint8_t i = REG_RAX; i < REG_MAX; i++)
        {
            terminalWrite(cpuRegToStr(i));
            terminalWrite(": ");
            terminalWriteHex(regs->vals[i]);
            terminalWrite("\n");
        }
    }
    else
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

        terminalWriteHex(*(uint32_t*)addr); // i don't care
    }

    terminalWrite("\n");

    return true;
}