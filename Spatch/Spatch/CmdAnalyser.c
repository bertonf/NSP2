#include <string.h>
#include "CmdAnalyser.h"

static char g_cmd[256];

void func1()
{}

CmdData CmdList[] = {
    {"lsserv", func1, USER},
    {"lsuser", func1, ADMIN},
    {"lsaccess", func1, ADMIN},
    {"connectto", func1, USER},
    {"createuser", func1, ADMIN},
    {"deleteuser", func1, ADMIN},
    {"createserv", func1, ADMIN},
    {"deleteserv", func1, ADMIN},
    {"createaccess", func1, ADMIN},
    {"deleteaccess", func1, ADMIN},
    {NULL, NULL, NONE}
};



void CmdAnalyserInit()
{
    memset(g_cmd, 0, 256);
}

int CmdExist()
{
    int i = 0;

    while (CmdList[i].name != NULL)
    {
        if (strcmp(g_cmd, CmdList[i].name) == 0)
            return (i);
        ++i;
    }
    return (-1);
}

static void ParsingCmd()
{
    int ret = 0;

    ret = CmdExist();
    if (ret == -1)
    {
        // VERIFIER SI CONNECTER
        // CMD TO SERV
    }
    else
    {
        //VERIFIER ACCESS
        CmdList[ret].func();
    }
}

void CmdAnalyser(char* cmd, int size)
{
    strncat(g_cmd, cmd, size);
    if (cmd[size] == '\n')
        ParsingCmd();
}
