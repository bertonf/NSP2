#include <string.h>
#include <stdio.h>
#include <libssh/libssh.h>
#include "CmdAnalyser.h"
#include "Spatch.h"

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

int CheckingAccessCmd(int id, usrData* uData)
{
    if (CmdList[id].access <= uData->spaAccess)
        return (0);
    return (1);
}

static void ParsingCmd(char* cmd, sessionData* sesData)
{
    int ret = 0;

    ret = CmdExist(cmd);
    if (ret == -1)
    {
        if (sesData->session == NULL)
        {
            ssh_channel_write(sesData->channel,
                              "Vous devez vous connecter a un serveur pour pouvoir utiliser cette commande.\n",
                              strlen("Vous devez vous connecter a un serveur pour pouvoir utiliser cette commande.\n"));
        }
        else
        {
            if (sesData->clientchannel == NULL)
                if ((sesData->clientchannel = open_client_channel(sesData->clientsession)) == NULL)
                        ssh_channel_write(sesData->channel,
                                          "Une erreur est survenue.\n",
                                          strlen("Une erreur est survenue.\n"));
            send_cmd_to_ssh(sesData->channel, sesData->clientchannel, cmd);
        }
    }
    else
    {
        printf("DEBUG | ParsingCmd() | EXIST\n");
        if (CheckingAccessCmd(ret, sesData->uData) != 0)
            ssh_channel_write(sesData->channel,
                              "Vous n'avez pas les autorisations nécessaire pour utiliser cette commande.\n",
                              strlen("Vous n'avez pas les autorisations nécessaire pour utiliser cette commande.\n"));
        else
        {
            //CmdList[ret].func();
        }
    }
}

void CmdAnalyser(char* cmd, sessionData* sesData)
{
    printf("DEBUG | CmdAnalyser() | CMD = [%s]\n", cmd);
    ParsingCmd(cmd, sesData);
}
