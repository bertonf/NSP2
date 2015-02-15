#include <string.h>
#include <stdio.h>
#include <libssh/libssh.h>
#include "CmdAnalyser.h"

void func1(char* cmd, ssh_channel chan)
{
    (void)cmd;
    ssh_channel_write(chan, "not implemented", strlen("not implemented"));

}

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

static int CmdExist(char* cmd)
{
    int i = 0;

    while (CmdList[i].name != NULL)
    {
        if (strncmp(cmd, CmdList[i].name, strlen(CmdList[i].name)) == 0)
            return (i);
        ++i;
    }
    return (-1);
}

static void ParsingCmd(char* cmd, ssh_channel chanusr)
{
    int ret = 0;

    ret = CmdExist(cmd);
    if (ret == -1)
    {
        printf("DEBUG | ParsingCmd() | DOESNT EXIST\n");
        // VERIFIER SI CONNECTER
        // CMD TO SERV
    }
    else
    {
        printf("DEBUG | ParsingCmd() | EXIST\n");
        //VERIFIER ACCESS
        //CmdList[ret].func();
    }
}

void CmdAnalyser(char* cmd, ssh_channel chanusr)
{
    printf("DEBUG | CmdAnalyser() | CMD = [%s]\n", cmd);
    ParsingCmd(cmd, chanusr);
}
