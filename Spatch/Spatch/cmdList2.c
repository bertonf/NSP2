#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

#include "cmdlist.h"
#include "svrFile.h"
#include "clientlistner.h"
#include "sendclientcmd.h"

void cmd_lsserv(char** cmdtab, sessionData* sesData)
{
    struct dirent *file;
    DIR *rep;
    char *filename;
    char *extfile;
    eaccess acc;
    char *msg;

    if(cmdtab[0] == NULL)
    {
        ssh_channel_write(sesData->channel,
                          "usage \"lsserv\"\r\n",
                          strlen("usage \"lsserv\"\r\n"));
        return;
    }
    rep = opendir(".");
    while ((file = readdir(rep)))
    {
        filename = strtok(file->d_name, ".");
        extfile = strtok(NULL, ".");
        if (filename != NULL && extfile != NULL)
        {
            if(strncmp(extfile, "svr", 3) == 0)
            {
                acc = isUsrAccess(sesData->uData, filename);
                switch(acc)
                {
                case NONE:
                    msg = malloc(strlen(filename)+strlen("\t|\tNONE\r\n") + 1);
                    memset(msg, 0, strlen(filename)+strlen("\t|\tNONE\r\n") + 1);
                    strcpy(msg, filename);
                    strcat(msg, "\t|\tNONE\r\n");
                    break;
                case USER:
                    msg = malloc(strlen(filename)+strlen("\t|\tUSER\r\n") + 1);
                    memset(msg, 0, strlen(filename)+strlen("\t|\tUSER\r\n") + 1);
                    strcpy(msg, filename);
                    strcat(msg, "\t|\tUSER\r\n");
                    break;
                case ADMIN:
                    msg = malloc(strlen(filename)+strlen("\t|\tADMIN\r\n") + 1);
                    memset(msg, 0, strlen(filename)+strlen("\t|\tADMIN\r\n") + 1);
                    strcpy(msg, filename);
                    strcat(msg, "\t|\tADMIN\r\n");
                    break;
                }
                ssh_channel_write(sesData->channel, msg, strlen(msg));
                free(msg);
            }
        }
    }
    closedir(rep);
}

void cmd_lsuser(char** cmdtab, sessionData* sesData)
{
    struct dirent *file;
    DIR *rep;
    char *filename;
    char *extfile;
    usrData *uData;
    eaccess acc;
    char *msg;

    if(cmdtab[0] == NULL || cmdtab[1] == NULL)
    {
        ssh_channel_write(sesData->channel,
                          "usage \"lsuser server\"\r\n",
                          strlen("usage \"lsuser server\"\r\n"));
        return;
    }
    rep = opendir(".");
    while ((file = readdir(rep)))
    {
        filename = strtok(file->d_name, ".");
        extfile = strtok(NULL, ".");
        if (filename != NULL && extfile != NULL)
        {
            if(strncmp(extfile, "usr", 3) == 0)
            {
                uData = GetUsrData(filename);
                acc = isUsrAccess(uData, cmdtab[1]);

                switch(acc)
                {
                case NONE:
                    msg = malloc(strlen(uData->name)+strlen("\t|\tNONE\r\n") + 1);
                    memset(msg, 0, strlen(uData->name)+strlen("\t|\tNONE\r\n") + 1);
                    strcpy(msg, uData->name);
                    strcat(msg, "\t|\tNONE\r\n");
                    break;
                case USER:
                    msg = malloc(strlen(uData->name)+strlen("\t|\tUSER\r\n") + 1);
                    memset(msg, 0, strlen(uData->name)+strlen("\t|\tUSER\r\n") + 1);
                    strcpy(msg, uData->name);
                    strcat(msg, "\t|\tUSER\r\n");
                    break;
                case ADMIN:
                    msg = malloc(strlen(uData->name)+strlen("\t|\tADMIN\r\n") + 1);
                    memset(msg, 0, strlen(uData->name)+strlen("\t|\tADMIN\r\n") + 1);
                    strcpy(msg, uData->name);
                    strcat(msg, "\t|\tADMIN\r\n");
                    break;
                }
                ssh_channel_write(sesData->channel, msg, strlen(msg));
                free(msg);
                FreeUsrData(uData);
            }
        }
    }
    closedir(rep);
}

void cmd_lsaccess(char** cmdtab, sessionData* sesData)
{
    usrData* uData;
    servAccess *sAccess;
    char *msg;

    if(cmdtab[0] == NULL || cmdtab[1] == NULL)
    {
        ssh_channel_write(sesData->channel,
                          "usage \"lsaccess user\"\r\n",
                          strlen("usage \"lsaccess user\"\r\n"));
        return;
    }
    uData = GetUsrData(cmdtab[1]);
    if (uData != NULL)
    {
        sAccess = uData->sAccess;
        while (sAccess != NULL)
        {
            switch(sAccess->acc)
            {
            case NONE:
                msg = malloc(strlen(sAccess->name)+strlen("\t|\tNONE\r\n") + 1);
                memset(msg, 0, strlen(sAccess->name)+strlen("\t|\tNONE\r\n") + 1);
                strcpy(msg, sAccess->name);
                strcat(msg, "\t|\tNONE\r\n");
                break;
            case USER:
                msg = malloc(strlen(sAccess->name)+strlen("\t|\tUSER\r\n") + 1);
                memset(msg, 0, strlen(sAccess->name)+strlen("\t|\tUSER\r\n") + 1);
                strcpy(msg, sAccess->name);
                strcat(msg, "\t|\tUSER\r\n");
                break;
            case ADMIN:
                msg = malloc(strlen(sAccess->name)+strlen("\t|\tADMIN\r\n") + 1);
                memset(msg, 0, strlen(sAccess->name)+strlen("\t|\tADMIN\r\n") + 1);
                strcpy(msg, sAccess->name);
                strcat(msg, "\t|\tADMIN\r\n");
                break;
            }
            ssh_channel_write(sesData->channel, msg, strlen(msg));
            free(msg);
            sAccess = sAccess->next;
        }
        FreeUsrData(uData);
    }
}


void cmd_connectto(char** cmdtab, sessionData* sesData)
{
    svrData* sData;
    (void)cmdtab;
    (void)sesData;

    if(cmdtab[0] == NULL || cmdtab[1] == NULL)
    {
        ssh_channel_write(sesData->channel,
                          "usage \"connectto server\"\r\n",
                          strlen("usage \"connectto server\"\r\n"));
        return;
    }

    sData = GetSvrData(cmdtab[1]);
    if (sData != NULL)
    {
        sesData->clientsession = connect_ssh(sData->ip, sData->port, sesData->uData->name,
                                             sesData->uData->pwd, 1);
        if (sesData->clientsession == NULL)
        {
            ssh_channel_write(sesData->channel,
                              "La connexion au serveur a echoué.\r\n",
                              strlen("La connexion au serveur a echoué.\r\n"));
        }
        else
        {
            ssh_channel_write(sesData->channel,
                              "Connexion au serveur réussi.\r\n",
                              strlen("Connexion au serveur réussi.\r\n"));
        }
    }
    else
    {
        ssh_channel_write(sesData->channel,
                          "Serveur inconnu.\r\n",
                          strlen("Serveur inconnu.\r\n"));
    }
}

void cmd_disconnect(char** cmdtab, sessionData* sesData)
{
    if(cmdtab[0] == NULL)
    {
        ssh_channel_write(sesData->channel,
                          "usage \"disconnect\"\r\n",
                          strlen("usage \"disconnect\"\r\n"));
        return;
    }
    if(sesData->clientsession == NULL)
    {
        ssh_channel_write(sesData->channel,
                          "Vous n'etes connecté a aucun serveur\r\n",
                          strlen("Vous n'etes connecté a aucun serveur\r\n"));
    }
    else
    {
        if (sesData->clientchannel != NULL)
            close_client_channel(sesData->clientchannel);

        ssh_disconnect(sesData->clientsession);
        ssh_free(sesData->clientsession);
        ssh_channel_write(sesData->channel,
                          "Vous etes deconnecté du serveur\r\n",
                          strlen("Vous etes deconnecté du serveur\r\n"));
        sesData->clientchannel = NULL;
        sesData->clientsession = NULL;
    }
}
