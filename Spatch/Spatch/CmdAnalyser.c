#include <string.h>
#include <stdio.h>
#include <libssh/libssh.h>
#include "CmdAnalyser.h"
#include "Spatch.h"
#include "cmdlist.h"

void func1(char** cmdtab, sessionData* sesData)
{
    (void)cmdtab;
    ssh_channel_write(sesData->channel, "not implemented", strlen("not implemented"));
}

CmdData CmdList[] = {
    {"lsserv", cmd_lsserv, USER},
    {"lsuser", cmd_lsuser, ADMIN},
    {"lsaccess", cmd_lsaccess, ADMIN},
    {"connectto", cmd_connectto, USER},
    {"createuser", cmd_createuser, ADMIN},
    {"deleteuser", cmd_deleteuser, ADMIN},
    {"createserv", cmd_createserver, ADMIN},
    {"deleteserv", cmd_deleteserver, ADMIN},
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

char** wordToTab(char* cmd)
{
    char** result = NULL;
    int nbword = 0;
    char* word = NULL;
    int i = 0;
    do
    {
        if (*cmd == ' ' || *cmd == '\n' || *cmd == '\0')
            ++nbword;
        ++cmd;
    } while (*cmd != '\0' || *cmd != '\n');

    result = malloc(sizeof(char*) * (nbword + 1));
    if (result != NULL)
    {
        word = strtok(cmd, " \n\0");
        while (word)
        {
            result[i++] = word;
            word = strtok(NULL, " \n\0");
        }
        result[i] = NULL;
    }
    return (result);
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
            CmdList[ret].func(wordToTab(cmd), sesData);
        }
    }
}

void CmdAnalyser(char* cmd, sessionData* sesData)
{
    printf("DEBUG | CmdAnalyser() | CMD = [%s]\n", cmd);
    ParsingCmd(cmd, sesData);
}
