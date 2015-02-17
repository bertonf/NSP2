#include <string.h>
#include <stdio.h>
#include <libssh/libssh.h>
#include "CmdAnalyser.h"
#include "Spatch.h"
#include "cmdlist.h"
#include "sendclientcmd.h"

void func1(char** cmdtab, sessionData* sesData)
{
    (void)cmdtab;
    ssh_channel_write(sesData->channel, "not implemented\r\n", strlen("not implemented\r\n"));
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
    {"createaccess", cmd_createaccess, ADMIN},
    {"deleteaccess", cmd_deleteaccess, ADMIN},
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

    while (cmd[i] != '\0')
    {
        if (cmd[i] == ' ' || cmd[i] == '\r' || cmd[i] == '\n' || cmd[i] == '\0')
            ++nbword;
        if (cmd[i] == '\r')
            cmd[i] = '\0';
        ++i;
    }
    ++nbword;
    i = 0;
    result = malloc(sizeof(char*) * (nbword + 1));
    if (result != NULL)
    {
        word = strtok(cmd, " \r\n\0");
        while (word)
        {
            result[i++] = word;
            word = strtok(NULL, " \r\n\0");
        }
        result[i] = NULL;
    }
    return (result);
}

void FormatCmd(char *cmd)
{
    printf("cmd[%d]=%d\n", strlen(cmd) - 1, cmd[strlen(cmd) - 1]);
    if (cmd[strlen(cmd) - 1] == '\r' || cmd[strlen(cmd) - 1] == '\n')
        cmd[strlen(cmd) - 1] = '\0';
}

static void ParsingCmd(char* cmd, sessionData* sesData)
{
    int ret = 0;

    ret = CmdExist(cmd);
    if (ret == -1)
    {
        if (sesData->clientsession == NULL)
        {
            ssh_channel_write(sesData->channel,
                              "Vous devez vous connectez a un serveur pour pouvoir utiliser cette commande.\r\n",
                              strlen("Vous devez vous connectez a un serveur pour pouvoir utiliser cette commande.\r\n"));
        }
        else
        {
            if (sesData->clientchannel == NULL)
            {
                sesData->clientchannel = open_client_channel(sesData->clientsession);
                if (sesData->clientchannel == NULL)
                {
                    ssh_channel_write(sesData->channel,
                                          "Une erreur est survenue.\r\n",
                                          strlen("Une erreur est survenue.\r\n"));
                    return;
                }
            }
            FormatCmd(cmd);
            send_cmd_to_ssh(sesData->channel, sesData->clientchannel, cmd);
            close_client_channel(sesData->clientchannel);
            sesData->clientchannel = NULL;
        }
    }
    else
    {
        printf("DEBUG | ParsingCmd() | EXIST\n");
        if (CheckingAccessCmd(ret, sesData->uData) != 0)
            ssh_channel_write(sesData->channel,
                              "Vous n'avez pas les autorisations nécessaire pour utiliser cette commande.\r\n",
                              strlen("Vous n'avez pas les autorisations nécessaire pour utiliser cette commande.\r\n"));
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
